// antighost_v2.c — anti‑ghost con promote_queued() che genera SOLO eventi sintetici
// e listener che aggiorna i buffer (pressed_buf) su press hardware + sintetiche

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/physical_layouts.h>
#include <zmk/matrix_transform.h>
#include <string.h>
#include <stdint.h>

LOG_MODULE_REGISTER(antighost_v2, LOG_LEVEL_DBG);

#define MAX_KEYS      256
#define BUFFER_SIZE   16
#define MAX_PRESSED   10  // massimo tasti simultanei gestiti

typedef struct {
    const uint32_t *lookup_table;
    size_t len;
    uint8_t rows, columns, col_offset, row_offset;
} zmk_matrix_transform_real_t;

//=== stati globali ===

static bool initialized;
static const zmk_matrix_transform_real_t *mt_global;
static size_t phys_len;

// buffer di “tasti reali” e di ghost in attesa
static uint8_t pressed_buf[BUFFER_SIZE];
static size_t pressed_count;
static uint8_t queued_buf[BUFFER_SIZE];
static size_t queued_count;

// work deferito per promuovere ghost (solo eventi, non tocca pressed_buf)
static struct k_work_delayable promote_work;

//=== forward ===
static void promote_fn(struct k_work *work);

//=== helper matrix transform ===

static const zmk_matrix_transform_real_t *get_mt(void) {
    const struct zmk_physical_layout *const *layouts;
    size_t cnt = zmk_physical_layouts_get_list(&layouts);
    int sel = zmk_physical_layouts_get_selected();
    if (sel < 0 || sel >= (int)cnt) {
        return NULL;
    }
    return (const zmk_matrix_transform_real_t *)layouts[sel]->matrix_transform;
}

static void antighost_v2_lazy_init(void) {
    mt_global = get_mt();
    if (!mt_global) {
        LOG_ERR("antighost_v2: no matrix transform");
        return;
    }
    phys_len      = MIN(mt_global->len, MAX_KEYS);
    pressed_count = queued_count = 0;
    initialized   = true;
    // init work
    k_work_init_delayable(&promote_work, promote_fn);
    LOG_INF("init: phys_len=%u rows=%u cols=%u",
            (uint32_t)phys_len, mt_global->rows, mt_global->columns);
}

//=== buffer helpers (dedup & cross‑remove) ===

static bool buf_contains(const uint8_t buf[], size_t cnt, uint8_t v) {
    for (size_t i = 0; i < cnt; i++) {
        if (buf[i] == v) {
            return true;
        }
    }
    return false;
}

static void buf_add(uint8_t buf[], size_t *cnt, uint8_t v) {
    if (buf_contains(buf, *cnt, v)) {
        return;  // già presente
    }
    if (*cnt >= BUFFER_SIZE) {
        // scroll out oldest
        memmove(buf, buf + 1, (*cnt - 1) * sizeof(buf[0]));
        (*cnt)--;
    }
    buf[(*cnt)++] = v;
}

static void buf_remove(uint8_t buf[], size_t *cnt, uint8_t v) {
    for (size_t i = 0; i < *cnt; i++) {
        if (buf[i] == v) {
            memmove(buf + i, buf + i + 1,
                    (*cnt - i - 1) * sizeof(buf[0]));
            (*cnt)--;
            return;
        }
    }
}

static void buf_clear(uint8_t buf[], size_t *cnt) {
    *cnt = 0;
}

//=== verifica ghost‑corner ===

static bool forms_square(uint8_t new_pos) {
    size_t phys_new = mt_global->len;
    for (size_t i = 0; i < mt_global->len; i++) {
        uint32_t v = mt_global->lookup_table[i];
        if (v && (v - 1) == new_pos) {
            phys_new = i;
            break;
        }
    }
    if (phys_new >= phys_len) return false;

    size_t cols = mt_global->columns;
    size_t rows = mt_global->rows;
    size_t r2 = phys_new / cols, c2 = phys_new % cols;

    for (size_t r1 = 0; r1 < rows; r1++) {
        if (r1 == r2) continue;
        for (size_t c1 = 0; c1 < cols; c1++) {
            if (c1 == c2) continue;
            size_t p00 = r1*cols + c1;
            size_t p01 = r1*cols + c2;
            size_t p10 = r2*cols + c1;
            size_t p11 = r2*cols + c2;
            uint8_t pos00 = mt_global->lookup_table[p00] ? mt_global->lookup_table[p00]-1 : 0xFF;
            uint8_t pos01 = mt_global->lookup_table[p01] ? mt_global->lookup_table[p01]-1 : 0xFF;
            uint8_t pos10 = mt_global->lookup_table[p10] ? mt_global->lookup_table[p10]-1 : 0xFF;
            uint8_t pos11 = mt_global->lookup_table[p11] ? mt_global->lookup_table[p11]-1 : 0xFF;

            int cnt = 0;
            for (size_t k = 0; k < pressed_count; k++) {
                uint8_t p = pressed_buf[k];
                if (p == new_pos) continue;
                if (p == pos00 || p == pos01 || p == pos10 || p == pos11) {
                    cnt++;
                }
            }
            if (cnt >= 2) {
                return true;
            }
        }
    }
    return false;
}

//=== promote_fn: genera SOLO eventi sintetici dai queued_buf ===

static void promote_fn(struct k_work *work) {
    ARG_UNUSED(work);

    // snapshot dei queued
    uint8_t snapshot[BUFFER_SIZE];
    size_t sc = queued_count;
    memcpy(snapshot, queued_buf, sc);

    for (size_t i = 0; i < sc; i++) {
        uint8_t q = snapshot[i];

        // se non è più ghost, lo promuoviamo
        if (!forms_square(q)) {
            // rimuovo da queued_buf
            buf_remove(queued_buf, &queued_count, q);

            // inietto evento sintetico: il listener lo aggiungerà a pressed_buf
            raise_zmk_position_state_changed((struct zmk_position_state_changed){
                .source   = ZMK_POSITION_STATE_CHANGE_SOURCE_LOCAL,
                .position = q,
                .state    = true,
                .timestamp= k_uptime_get(),
            });
            LOG_DBG("promote_fn: raised synthetic press %u", q);
        }
    }

    // se non ci sono pressed veri e restano queued, pulisco
    if (pressed_count == 0 && queued_count > 0) {
        buf_clear(queued_buf, &queued_count);
        LOG_DBG("promote_fn: cleared queued_buf (no pressed)");
    }
}

//=== listener principale ===

static int antighost_v2_listener(const zmk_event_t *eh) {
    struct zmk_position_state_changed *ev =
        as_zmk_position_state_changed(eh);
    if (!ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    if (!initialized) {
        antighost_v2_lazy_init();
        if (!initialized) {
            return ZMK_EV_EVENT_BUBBLE;
        }
    }

    uint8_t pos = ev->position;

    if (ev->state) {
        // -------- PRESS (hardware o synthetic)

        // rimuovo da queued_buf (se c’è, pulisco doppioni)
        buf_remove(queued_buf, &queued_count, pos);

        // limite MAX_PRESSED
        if (pressed_count >= MAX_PRESSED) {
            LOG_DBG("blocked press %u: too many pressed", pos);
            return ZMK_EV_EVENT_HANDLED;
        }

        // ghost?
        if (forms_square(pos)) {
            buf_add(queued_buf, &queued_count, pos);
            LOG_DBG("queued ghost %u", pos);
            return ZMK_EV_EVENT_HANDLED;
        }

        // altrimenti accetto e aggiungo a pressed_buf
        buf_add(pressed_buf, &pressed_count, pos);
        LOG_DBG("accepted press %u", pos);
        return ZMK_EV_EVENT_BUBBLE;

    } else {
        // -------- RELEASE

        // rilascio tasto reale?
        if (buf_contains(pressed_buf, pressed_count, pos)) {
            buf_remove(pressed_buf, &pressed_count, pos);
            LOG_DBG("removed %u from pressed_buf", pos);

            // promuovo i queued immediatamente
            k_work_schedule(&promote_work, K_NO_WAIT);
            return ZMK_EV_EVENT_BUBBLE;
        }

        // rilascio ghost in queued?
        if (buf_contains(queued_buf, queued_count, pos)) {
            buf_remove(queued_buf, &queued_count, pos);
            LOG_DBG("removed %u from queued_buf", pos);
            return ZMK_EV_EVENT_HANDLED;
        }

        // altrimenti nessuna traccia
        return ZMK_EV_EVENT_BUBBLE;
    }
}

ZMK_LISTENER(antighost_v2, antighost_v2_listener);
ZMK_SUBSCRIPTION(antighost_v2, zmk_position_state_changed);
