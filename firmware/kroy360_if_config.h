#ifndef KROY360_IF_CONFIG_H
#define KROY360_IF_CONFIG_H

#include <avr/io.h>
#include <avr/interrupt.h>

/* Pin definitions */
#define DATA_PINS   PINB
#define DATA_PORT   PORTB
#define DATA_DDR    DDRB

#define LED_PINS    PINF
#define LED_PORT    PORTF
#define LED_DDR     DDRF
#define CLLED_PIN   PF1
#define TSLED_PIN   PF4
#define EMLED_PIN   PF5
#define STLED_PIN   PF7
#define LED_BITS    ((1 << CLLED_PIN) | (1 << TSLED_PIN) | \
                     (1 << EMLED_PIN) | (1 << STLED_PIN))


#define PS2_PIN     PF6
#define PS2_PORT    PORTF
#define PS2_DDR     DDRF


#define PS1_PINS    PIND
#define PS1_PIN     PD0
#define PS1_PORT    PORTD
#define PS1_DDR     DDRD
#define PS1_INT     INT0_vect


#define KBEN_PINS    PINC
#define KBEN_PIN     PD6
#define KBEN_PORT    PORTC
#define KBEN_DDR     DDRC

typedef enum kroy360_led_e
{
    KROY360_LED_CL = (1 << CLLED_PIN),
    KROY360_LED_TS = (1 << TSLED_PIN),
    KROY360_LED_EM = (1 << EMLED_PIN),
    KROY360_LED_ST = (1 << STLED_PIN),
    KROY360_LED_ALL = LED_BITS,
} kroy360_led_e;

#endif
