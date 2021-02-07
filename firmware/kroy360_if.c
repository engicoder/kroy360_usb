/*
Copyright 2019 engicoder <engicoder@engicoder.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/


#include <stdbool.h>
#include "kroy360_if.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "print.h"
#include "debug.h"
#include "util.h"





static inline void kb_data_init(void)
{
    DATA_DDR = 0;
}

static inline uint8_t kb_data_read(void)
{
    return DATA_PINS;
}

static inline void kb_en_init(void)
{
    KBEN_PORT &= ~(1 << KBEN_PIN);
    KBEN_DDR |= (1 << KBEN_PIN);
}

static inline void kb_en_enable(void)
{
    KBEN_PORT |= (1 << KBEN_PIN);
}

static inline void kb_en_disable(void)
{
    KBEN_PORT &= ~(1 << KBEN_PIN);
}


static inline void ps1_init(void)
{
    PS1_PORT |= (1 << PS1_PIN);
    PS1_DDR &= ~(1 << PS1_PIN);

    /* Disable interrupt via mask register */
    EIMSK &= ~(1 << INT0);

    /* Set INT1 to trigger on falling edge */
    EICRA &= ~((1 << ISC01) | (1 << ISC00));
    EICRA |= ((1 << ISC01) | (0 << ISC00));

    /* Clear any outstanding flag */
    EIFR = (1 << INTF0);
}

static inline void ps1_int_enable(void)
{
    /* Clear any outstanding flag */
    EIFR = (1 << INTF0);
    /* Enable interrupt in mask register */
    EIMSK |= (1 << INT0);
}

static inline void ps1_int_disable(void)
{
    /* Disable interrupt via mask register */
    EIMSK &= ~(1 << INT0);
}



static inline void ps2_init(void)
{
    PS2_PORT |= (1 << PS2_PIN);
    PS2_DDR |= (1 << PS2_PIN);
}

static inline void ps2_low(void)
{
    PS2_PORT &= ~(1 << PS2_PIN);
}
#define TIMER_INT TIMER3_COMPB_vect

#define BAUD_RATE 2400
#define TIMER_PRESCALER 8
#define TIMER_CLOCK F_CPU/TIMER_PRESCALER
#define TIMER_PERIOD 832

static inline void ps2_high(void)
{
    PS2_PORT |= (1 << PS2_PIN);
}


static inline void led_init(void)
{
    LED_PORT |= LED_BITS;
    LED_DDR |= LED_BITS;
}


static inline void led_on(kroy360_led_e led)
{
    LED_PORT &= ~led;
}

static inline void led_off(kroy360_led_e led)
{
    LED_PORT |= led;
}

static inline void led_toggle(kroy360_led_e led)
{
    LED_PORT ^= led;
}

void led_update(uint8_t usb_led)
{
    xprintf("Leds: %2X", usb_led);
    if (usb_led & (1 << USB_LED_CAPS_LOCK))
        led_on(KROY360_LED_CL);
    else
        led_off(KROY360_LED_CL);
}

#define TIMER_PRESCALER 8
#define TIMER_CLOCK F_CPU/TIMER_PRESCALER
#define TIMER_PERIOD_A 200
#define TIMER_PERIOD_B 34
#define TIMER_INTA TIMER3_COMPA_vect
#define TIMER_INTB TIMER3_COMPB_vect


static void timer_init(void)
{
    /* Configure Timer3 */
    TCCR3A = 0;

	/* Enable CTC mode and stop timer by selecting no clock source*/
    TCCR3A = 0x00;
    TCCR3B = (1 << WGM32);

    OCR3A = TIMER_PERIOD_A;
    OCR3B = TIMER_PERIOD_B;

	/* Reset counter */
    TCNT3 = 0;
}


static void timer_int_enable(void)
{
    /* Reset counter */
    TCNT3 = 0;

    /* Clear the Compare Match flags*/
    TIFR3 = (1<<OCF3A);
    TIFR3 = (1<<OCF3B);
    /* Enable interrupt on Compare Match */
    TIMSK3 |= 1 << OCIE3A;
    TIMSK3 |= 1 << OCIE3B;
    /* Start timer by selecting CLk/8 source */
    TCCR3B |= (1 << CS31);
}


static void timer_int_disable(void)
{
    /* Stop timer by selecting no clock source but leaving CTC mode*/
    TCCR3B = (1 << WGM32);

    /* Disable Compare Match interrupts */
    TIMSK3 &= ~(1 << OCIE3A);
    TIMSK3 &= ~(1 << OCIE3B);

    /* Clear the Compare Match flags*/
    TIFR3 = (1<<OCF3A);
    TIFR3 = (1<<OCF3B);

}



static inline uint8_t pbuf_dequeue(void);
static inline void pbuf_enqueue(uint8_t data);
static inline bool pbuf_has_data(void);
static inline void pbuf_clear(void);

void kroy360_if_init(void)
{
    pbuf_clear();

    kb_data_init();
    led_init();
    ps1_init();
    ps2_init();
    timer_init();
    kb_en_enable();
    _delay_ms(10);
    led_on(KROY360_LED_ALL);
    _delay_ms(250);
    led_off(KROY360_LED_ALL);
    ps1_int_enable();
}


uint8_t kroy360_if_recv(void)
{
    uint8_t data = 0;

    if (pbuf_has_data())
    {
        led_on(KROY360_LED_ST);
        data = pbuf_dequeue();
        led_off(KROY360_LED_ST);
    }

    return data;
}

bool kroy360_if_has_data(void)
{
    return pbuf_has_data();
}

void kroy360_if_led_toggle(kroy360_led_e led)
{
    led_toggle(led);
}


ISR(PS1_INT)
{
    timer_int_enable();
    uint8_t data = kb_data_read();
    pbuf_enqueue(data);
}



ISR(TIMER_INTB)
{
    ps2_low();
}

ISR(TIMER_INTA)
{
    timer_int_disable();
    ps2_high();
}


 #define PBUF_SIZE 32
static uint8_t pbuf[PBUF_SIZE];
static uint8_t pbuf_head = 0;
static uint8_t pbuf_tail = 0;

static inline void pbuf_enqueue(uint8_t data)
{
    uint8_t sreg = SREG;
    cli();
    uint8_t next = (pbuf_head + 1) % PBUF_SIZE;
    if (next != pbuf_tail) {
        pbuf[pbuf_head] = data;
        pbuf_head = next;
    }
    SREG = sreg;
}
static inline uint8_t pbuf_dequeue(void)
{
    uint8_t val = 0;

    uint8_t sreg = SREG;
    cli();
    if (pbuf_head != pbuf_tail)
    {
        val = pbuf[pbuf_tail];
        pbuf_tail = (pbuf_tail + 1) % PBUF_SIZE;
    }
    SREG = sreg;

    return val;
}
static inline bool pbuf_has_data(void)
{
    uint8_t sreg = SREG;
    cli();
    bool has_data = (pbuf_head != pbuf_tail);
    SREG = sreg;
    return has_data;
}
static inline void pbuf_clear(void)
{
    uint8_t sreg = SREG;
    cli();
    pbuf_head = pbuf_tail = 0;
    SREG = sreg;
}

