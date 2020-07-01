/*
Copyright 2016 Engicoder
*/


#ifndef KROY360_IF_H
#define KROY360_IF_H

#include <stdint.h>
#include "led.h"
#include "kroy360_if_config.h"

void kroy360_if_init(void);
bool kroy360_if_has_data(void);
uint8_t kroy360_if_recv(void);
void kroy360_if_led_toggle(kroy360_led_e led);

#define DATA_MASK   0x7F
static inline bool kroy360_if_pressed(uint8_t data)
{
    return (data & ~DATA_MASK) != 0;
}

static inline uint8_t kroy360_if_scan_code(uint8_t data)
{
    return (data & DATA_MASK);
}

void led_update(uint8_t usb_led);



#endif // FORTUNE_H
