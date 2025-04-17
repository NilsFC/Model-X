#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/hid_indicators.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/keymap.h>
#include <zmk/physical_layouts.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

 // Define the layer ID for the numpad layer
 #define SSK_NUMPAD_LAYER 2

// Global variables
int current_layout = 0;
bool numlock = false;

// Function to handle numpad layer activation/deactivation based on Num Lock and layout
static void ssk_numpad_layer(void) {
    if ((numlock == 1) && (current_layout == 3 || current_layout == 4)) {
        zmk_keymap_layer_activate(SSK_NUMPAD_LAYER);
        LOG_INF("Activating SSK_NUMPAD_LAYER");
    } else {
        zmk_keymap_layer_deactivate(SSK_NUMPAD_LAYER);
        LOG_INF("Deactivating SSK_NUMPAD_LAYER");
    }
}

// Listener callback to handle changes in lock indicator states (e.g., Num Lock, Caps Lock, Scroll Lock)
static int hid_indicators_callback(const zmk_event_t *eh) {
    zmk_hid_indicators_t flags = zmk_hid_indicators_get_current_profile();
    numlock = flags & 0x01;
    ssk_numpad_layer();
	
    return ZMK_EV_EVENT_BUBBLE; //Propagate the event
}

// Register the listener to respond to HID indicator changes
ZMK_LISTENER(hid_indicators_listener, hid_indicators_callback);
ZMK_SUBSCRIPTION(hid_indicators_listener, zmk_hid_indicators_changed);


// Listener callback to handle physical layout changes
static int layout_selection_callback(const zmk_event_t *eh) {
    if (!eh) { // Ensure the event is valid
        LOG_ERR("Received null event");
        return -EINVAL;
    }
    current_layout = zmk_physical_layouts_get_selected();
    ssk_numpad_layer();

    return ZMK_EV_EVENT_BUBBLE;  //Propagate the event
}

// Register the listener to respond to layout selection changes
ZMK_LISTENER(layout_selection_listener, layout_selection_callback);
ZMK_SUBSCRIPTION(layout_selection_listener, zmk_physical_layout_selection_changed);


// Initialization function to set up the initial state of the LEDs and variables on boot
static int numpad_layout_init(void) {
    current_layout = zmk_physical_layouts_get_selected();
    return 0;
}

// Register the initialization function to run at boot time
SYS_INIT(numpad_layout_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
