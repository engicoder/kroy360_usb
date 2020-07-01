#include <stdint.h>
#include <stdbool.h>

#include <avr/interrupt.h>
#include <util/delay.h>

#include "h2525.h"
#include "h2525_pins.h"


#define LCD_ROWS 20
#define LCD_COLS 240

volatile uint8_t gram[LCD_ROWS*LCD_COLS/8] = {0};
volatile uint8_t col_idx = 0;
volatile uint8_t row = 0;
volatile bool m_pos = false;


static void init_pins(void)
{
    PIN_CONFIG_OUT_LOW(H2525_FLM);
    PIN_CONFIG_OUT_LOW(H2525_M);
    PIN_CONFIG_OUT_LOW(H2525_CL1);
}




#define BAUD 400000
#define BAUD_VALUE ((F_CPU)/(2UL*BAUD)-1)

#define USART_XCK H2525_CL2

#ifndef UCPHA1
#define UCPHA1 1
#endif

/* USART SPIM implemenation
 * ---------------------------------------------------------------------- */
void usart_spim_init(void)
{
    // Baud rate must be set to 0 prior to enabling the USART as SPI
    // master, to ensure proper initialization of the XCK line.
    UBRR1 = 0;

    // Set XCK line to output, ie. set USART in master mode.
    DDRD |= (1 << PD5);

    // Set USART to Master SPI mode.
    UCSR1C = (1 << UMSEL11) | (1 << UMSEL10) | (0 << UCPHA1) | (1 << UCPOL1);
}

void usart_spim_int_enable(void)
{
    col_idx = 0;
    row = 0;
    m_pos = false;
    PIN_HIGH(H2525_FLM);

    UCSR1B = (1 << TXEN1) | (1 << TXCIE1) | (1 << UDRIE1);
    UBRR1 = BAUD_VALUE;
    sei();
}

ISR(USART1_TX_vect)
{

    row++;
    if (row == LCD_ROWS)
        PIN_HIGH(H2525_FLM);
    _delay_us(2);
    PIN_HIGH(H2525_CL1);
    _delay_us(1);
    col_idx = 0;
    if(row == LCD_ROWS)
    {
        m_pos = !m_pos;
        if (m_pos)
            PIN_HIGH(H2525_M);
        else
            PIN_LOW(H2525_M);
        _delay_us(1);
    }
    PIN_LOW(H2525_CL1);
    _delay_us(1);
    if(row == LCD_ROWS)
    {
        PIN_LOW(H2525_FLM);
        row = 0;
    }
    UCSR1B |= ((1 <<TXEN1) |(1 << UDRIE1));
}


ISR(USART1_UDRE_vect)
{
/*
    if (col_idx == 2)
        PIN_LOW(H2525_FLM);
    else if ((row == LCD_ROWS-1) && (col_idx == LCD_COLS/8 - 2))
        PIN_HIGH(H2525_FLM);
    else if (col_idx == LCD_COLS/8 - 1)
        PIN_HIGH(H2525_CL1);
*/
    if (col_idx < LCD_COLS/8 - 1)
    {
        /*
        uint8_t data = gram[row * col_idx];
        if (m_pos)
            UDR1 = data;
        else
            UDR1 = ~data;
        */
        UDR1 = gram[row * LCD_COLS/8 + col_idx];
        col_idx++;
    }
    else
    {
        UDR1 = gram[row * LCD_COLS/8 + col_idx];
        col_idx++;
        UCSR1B &= ~((1 <<TXEN1) |(1 << UDRIE1));
    }

}


void h2525_init(void)
{
    init_pins();
//    init_timer();
    usart_spim_init();
}

void h2525_start(void)
{
//    timer_int_enable();
    usart_spim_int_enable();
}

void h2525_clear(void)
{
    for(int i = 0; i < sizeof(gram); i++)
    {
        gram[i] = 0;
    }
}

void h2525_set_pixel(uint8_t x, uint8_t y)
{
    uint16_t pixel_index = y * LCD_COLS + x;
    uint16_t  pixel_byte = pixel_index / 8;
    uint8_t pixel_bit = pixel_index % 8;

    gram[pixel_byte] |= (1 << (7-pixel_bit));
}
void h2525_clear_pixel(uint8_t x, uint8_t y)
{
    uint16_t pixel_index = y * LCD_COLS + x;
    uint16_t  pixel_byte = pixel_index / 8;
    uint8_t pixel_bit = pixel_index % 8;

    gram[pixel_byte] &= ~(1 << (7-pixel_bit));
}

void h2525_clear_region(uint8_t start_x, uint8_t start_y, uint8_t width, uint8_t height)
{
    uint8_t end_x = start_x + width;
    uint8_t end_y = start_y + height;

    for (uint8_t y = start_y; start_y < end_y; y++)
    {
        for (uint8_t x = start_x; x < end_x; x++)
        {
            h2525_clear_pixel(x, y);
        }
    }
}


#if 0
#define TIMER_PERIOD_A 120
#define TIMER_PERIOD_B TIMER_PERIOD_A/2
static void init_timer(void)
{
    /* Enable CTC mode and stop timer by selecting no clock source*/
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);

    OCR1A = TIMER_PERIOD_A;
    OCR1B = TIMER_PERIOD_B;

    /* Reset counter */
    TCNT1 = 0;
}

static void timer_int_enable(void)
{
    /* Reset counter */
    TCNT1 = 0;

    /* Clear the Compare Match flags*/
    TIFR1 = (1 << OCF1A);
    TIFR1 = (1 << OCF1B);

    /* Enable interrupt on Compare Match */
    TIMSK1 |= 1 << OCIE1A;
    TIMSK1 |= 1 << OCIE1B;

    /* Start timer by selecting CLk source  (1 presecaler)*/
    TCCR1B |= (1 << CS10);
}

#if 0
static void timer_int_disable(void)
{
    /* Stop timer by selecting no clock source but leaving CTC mode*/
    TCCR1B = (1 << WGM12);

    /* Disable Compare Match interrupts */
    TIMSK1 &= ~(1 << OCIE1A);
    TIMSK1 &= ~(1 << OCIE1B);

    /* Clear the Compare Match flags*/
    TIFR1 = (1<<OCF1A);
    TIFR1 = (1<<OCF1B);

}
#endif

#define TIMER_INTA TIMER1_COMPA_vect
#define TIMER_INTB TIMER1_COMPB_vect
volatile uint8_t byte_idx = LCD_COLS/8 - 1;
volatile uint8_t bit = (1 << 6);
volatile uint8_t row = LCD_ROWS - 1;
volatile uint8_t byte_curr = 0xAA;
/* Compare B sets CL2 low */
ISR(TIMER_INTB)
{
    if ((row == LCD_ROWS - 1) && (byte_idx == LCD_COLS/8 - 1) && bit == (1 << 6))
        PIN_HIGH(H2525_FLM);

    if ((byte_idx == LCD_COLS/8 - 1) && (bit == (1 << 7)))
    {
        PIN_HIGH(H2525_CL1);
        byte_idx = 0;
        bit = 1;
        row++;
        byte_curr = gram[byte_idx];
        if (row == LCD_ROWS)
            row = 0;
    }
    else
    {
        if (bit == (1 << 7))
        {
            bit = 1;
            byte_idx++;
            byte_curr = gram[byte_idx];
        }
        else
            bit <<= 1;
    }

    PIN_LOW(H2525_CL2);
}

/* Compare B sets CL2 high */
ISR(TIMER_INTA)
{
    if (byte_idx == 0 && bit == 1)
    {
        PIN_LOW(H2525_CL1);
        if (row == 0)
            PIN_TOGGLE(H2525_M);
    }
    else if ((row == 0) && (byte_idx == 0) && (bit == (1 << 5)))
    {
        PIN_LOW(H2525_FLM);
    }

    uint8_t pixel = byte_curr & bit;
    if (pixel == 0)
        PIN_LOW(H2525_D1);
    else
        PIN_HIGH(H2525_D1);
    PIN_HIGH(H2525_CL2);
}
#endif
