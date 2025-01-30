#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/hid_indicators.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/keymap.h>
#include <zmk/physical_layouts.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define LED_GPIO_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds)
#define SSK_NUMPAD_LAYER 2

// PWM configuration for Num Lock, Caps Lock, and Scroll Lock LEDs
static const struct pwm_dt_spec pwm_led_numlock = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_numlock));
static const struct pwm_dt_spec pwm_led_capslock = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_capslock));
static const struct pwm_dt_spec pwm_led_scrolllock = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led_scrolllock));

#define PWM_PERIOD_NS  20000000 // 20ms period (50Hz)
#define IDLE_BRIGHTNESS_PERCENT 25

// Global variables to store the current brightness for each LED
static uint8_t current_brightness_numlock = 0;
static uint8_t current_brightness_capslock = 0;
static uint8_t current_brightness_scrolllock = 0;

// Work structure for handling asynchronous LED updates
static struct k_work led_update_work;
static struct k_timer led_update_timer;

// Directly set PWM brightness for a given LED
static int direct_pwm_set(const struct pwm_dt_spec *pwm_spec, uint8_t brightness_percent, uint8_t *current_brightness) {
    if (brightness_percent > 100) {
        brightness_percent = 100;
    }

    // Calculate the duty cycle in nanoseconds based on the percentage
    uint32_t duty_cycle_ns = (brightness_percent * PWM_PERIOD_NS) / 100;

    // Set the PWM with the calculated duty cycle
    int err = pwm_set_dt(pwm_spec, PWM_PERIOD_NS, duty_cycle_ns);
    if (err) {
        LOG_ERR("Error setting PWM: %d", err);
        return err;
    }

    // Update the current brightness
    *current_brightness = brightness_percent;

    LOG_INF("Set PWM brightness to %d%% for device %s", brightness_percent, pwm_spec->dev->name);
    return 0;
}

// Function to update the LED brightness asynchronously
static void led_update_work_handler(struct k_work *work) {
    direct_pwm_set(&pwm_led_numlock, current_brightness_numlock, &current_brightness_numlock);
    direct_pwm_set(&pwm_led_capslock, current_brightness_capslock, &current_brightness_capslock);
    direct_pwm_set(&pwm_led_scrolllock, current_brightness_scrolllock, &current_brightness_scrolllock);
}

// Function called when the keyboard enters idle
static void on_idle(void) {
    printk("Keyboard is idle. Dimming LED brightness to %d%%.\n", IDLE_BRIGHTNESS_PERCENT);

    // Adjust brightness for idle state
    current_brightness_numlock = (current_brightness_numlock * IDLE_BRIGHTNESS_PERCENT) / 100;
    current_brightness_capslock = (current_brightness_capslock * IDLE_BRIGHTNESS_PERCENT) / 100;
    current_brightness_scrolllock = (current_brightness_scrolllock * IDLE_BRIGHTNESS_PERCENT) / 100;

    // Schedule the LED update asynchronously
    k_timer_start(&led_update_timer, K_MSEC(100), K_NO_WAIT); // Delay of 100ms before applying the dimming
}

// Function called when the keyboard wakes up from idle
static void on_wake_up(void) {
    printk("Keyboard has woken up from idle. Restoring LED brightness.\n");

    // Restore the original brightness of the LEDs based on the lock indicators
    zmk_hid_indicators_t flags = zmk_hid_indicators_get_current_profile();

    current_brightness_numlock = (flags & 0x01) ? 100 : 0;
    current_brightness_capslock = (flags & 0x02) ? 100 : 0;
    current_brightness_scrolllock = (flags & 0x04) ? 100 : 0;

    // Schedule the LED update asynchronously
    k_timer_start(&led_update_timer, K_MSEC(100), K_NO_WAIT); // Delay of 100ms before restoring the brightness
}

// Timer handler for delayed LED updates
static void led_update_timer_handler(struct k_timer *timer_id) {
    k_work_submit(&led_update_work);
}

// Callback for the activity state listener
static int on_activity_state_changed(const zmk_event_t *ev) {
    const struct zmk_activity_state_changed *activity_event = as_zmk_activity_state_changed(ev);

    if (!activity_event) {
        return -1;
    }

    if (activity_event->state == ZMK_ACTIVITY_IDLE) {
        on_idle();  // Handle idle state
    } else if (activity_event->state == ZMK_ACTIVITY_ACTIVE) {
        on_wake_up();  // Handle wake-up state
    }

    return ZMK_EV_EVENT_BUBBLE; //Propagate the event
}

// Listener for idle and wake-up state
ZMK_LISTENER(activity_state_changed_listener, on_activity_state_changed);
ZMK_SUBSCRIPTION(activity_state_changed_listener, zmk_activity_state_changed);

// Listener to handle changes in lock state (Num, Caps, Scroll Lock)
static int led_locks_listener_cb(const zmk_event_t *eh) {
    zmk_hid_indicators_t flags = zmk_hid_indicators_get_current_profile();

    // Define brightness levels for Num Lock, Caps Lock, and Scroll Lock
    current_brightness_numlock = (flags & 0x01) ? 100 : 0;
    current_brightness_capslock = (flags & 0x02) ? 100 : 0;
    current_brightness_scrolllock = (flags & 0x04) ? 100 : 0;

    // Schedule the LED update asynchronously
    k_timer_start(&led_update_timer, K_MSEC(100), K_NO_WAIT);

    LOG_INF("NUMLOCK is %d", current_brightness_numlock > 0);
    LOG_INF("CAPSLOCK is %d", current_brightness_capslock > 0);
    LOG_INF("SCROLLLOCK is %d", current_brightness_scrolllock > 0);

    return ZMK_EV_EVENT_BUBBLE; //Propagate the event;
}

ZMK_LISTENER(led_indicators_listener, led_locks_listener_cb);
ZMK_SUBSCRIPTION(led_indicators_listener, zmk_hid_indicators_changed);


// Initialize the LEDs and work structures on boot
static int leds_init(void) {
    if (!pwm_is_ready_dt(&pwm_led_numlock)) {
        LOG_ERR("Error: PWM device %s is not ready\n", pwm_led_numlock.dev->name);
        return -ENODEV;
    }
    
    if (!pwm_is_ready_dt(&pwm_led_capslock)) {
        LOG_ERR("Error: PWM device %s is not ready\n", pwm_led_capslock.dev->name);
        return -ENODEV;
    }

    if (!pwm_is_ready_dt(&pwm_led_scrolllock)) {
        LOG_ERR("Error: PWM device %s is not ready\n", pwm_led_scrolllock.dev->name);
        return -ENODEV;
    }

    // Initialize k_work and k_timer
    k_work_init(&led_update_work, led_update_work_handler);
    k_timer_init(&led_update_timer, led_update_timer_handler, NULL);
    
    return 0;
}

// Register leds_init to run at boot
SYS_INIT(leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
