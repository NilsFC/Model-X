#include <dt-bindings/zmk/hid_usage.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/keymap.h>
#include <zephyr/logging/log.h>
#include <zmk/hid.h>
#include <zmk/behavior.h>
#include <zephyr/kernel.h> // Per k_uptime_get()
#include <zephyr/sys/util.h>
#include <zmk/physical_layouts.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

 // Define the layer ID for the numpad layer
 #define SSK_NUMPAD_LAYER 2

// Funzione per triggerare una behavior
void trigger_behavior(const char *behavior_name, uint32_t param1, uint32_t param2, int layer, uint32_t position, bool pressed) {
    const struct device *behavior_dev = zmk_behavior_get_binding(behavior_name);
    if (!behavior_dev) {
        printk("Behavior %s not found!\n", behavior_name);
        return;
    }

    struct zmk_behavior_binding binding = {
        .behavior_dev = behavior_name,
        .param1 = param1,
        .param2 = param2,
    };

    struct zmk_behavior_binding_event event = {
        .layer = layer,
        .position = position,
        .timestamp = k_uptime_get(),
    };

    int ret = zmk_behavior_invoke_binding(&binding, event, pressed);
    if (ret < 0) {
        printk("Failed to invoke behavior %s: %d\n", behavior_name, ret);
    }
}

// Stato della combo
static bool combo_active = false;

// Lavoro ritardato per simulare il rilascio del tasto
static void delayed_release_work_handler(struct k_work *work) {
    trigger_behavior("key_press", HID_USAGE_KEY_KEYPAD_NUM_LOCK_AND_CLEAR, 0, 0, 1, false); // Rilascia "A"
    trigger_behavior("key_press", HID_USAGE_KEY_KEYBOARD_SCROLL_LOCK, 0, 0, 1, false); // Premi "A"
}

K_WORK_DELAYABLE_DEFINE(delayed_release_work, delayed_release_work_handler);

// Callback per la combo
static int combo_callback(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *event = as_zmk_keycode_state_changed(eh);

    if (!event) {
        LOG_ERR("Evento non valido.");
        return -EINVAL;
    }

    static bool left_shift_pressed = false;
    static bool right_shift_pressed = false;
    static bool scroll_lock_pressed = false;

    // Aggiorna lo stato dei tasti
    if (event->keycode == HID_USAGE_KEY_KEYBOARD_LEFTSHIFT) {
        left_shift_pressed = event->state;
    }

    if (event->keycode == HID_USAGE_KEY_KEYBOARD_RIGHTSHIFT) {
        right_shift_pressed = event->state;
    }

    if (event->keycode == HID_USAGE_KEY_KEYBOARD_SCROLL_LOCK) {
        scroll_lock_pressed = event->state;
    }
    
    int current_layout = zmk_physical_layouts_get_selected();
   
    // Attiva la combo solo una volta
    if ((current_layout == 3 || current_layout == 4) && !combo_active && scroll_lock_pressed && (left_shift_pressed || right_shift_pressed)) {

        combo_active = true;
        LOG_INF("Combo attivata: Shift + Scroll Lock");

        // Premi il tasto "A"
        trigger_behavior("key_press", HID_USAGE_KEY_KEYPAD_NUM_LOCK_AND_CLEAR, 0, 0, 1, true); // Premi "A"
        trigger_behavior("key_press", HID_USAGE_KEY_KEYBOARD_SCROLL_LOCK, 0, 0, 1, true); // Premi "A"

        // Pianifica il rilascio dopo 200ms
        k_work_schedule(&delayed_release_work, K_MSEC(200));
    }

    // Disattiva la combo quando i tasti sono rilasciati
    if (!scroll_lock_pressed) {
        combo_active = false;
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(combo_listener, combo_callback);
ZMK_SUBSCRIPTION(combo_listener, zmk_keycode_state_changed);
