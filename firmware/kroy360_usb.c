#include "kroy360_usb.h"

#include "kroy360_if.h"
#include "led.h"
#include "display.h"


void led_set(uint8_t usb_led)
{
    led_update(usb_led);
}


void matrix_init_kb(void) {
}

void matrix_scan_kb(void) {
}

void keyboard_post_init_kb(void)
{
    debug_enable=true;
    debug_matrix=false;   

    display_init();

    xprintf("Kroy 360 started\n");    
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record)
{
    if (!record->event.pressed) {
        switch (keycode) {
            case PWD_TOG:
                display_toggle_pwd_blanking();
                kroy360_if_led_toggle(KROY360_LED_TS);
                return false;
                break;
            default:
                display_on_hid_code(keycode);
                break;
        }
    }


    return process_record_user(keycode, record);
}
