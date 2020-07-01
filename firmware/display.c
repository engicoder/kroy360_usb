#include <stdint.h>
#include <stdbool.h>

#include <avr/interrupt.h>
#include "display.h"
#include "h2525.h"
#include "6x8_horizontal_lsb_1.h"
#include "usb_hid_codes.h"

static bool pwd_blanking_on = false;

uint8_t asciimap_symbols[]=
{
    ' ', '-', '=', '[', ']', '\\', '#', ';', '\'', '`', ',', '.', '/',
};

uint8_t asciimap_symbols_shift[]=
{
    ' ', '_', '+', '{', '}', '|', '~', ':', '"', '~', '<', '>', '?',
};

uint8_t asciimap_numbers_shift[]=
{
    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
};

const uint8_t special[][8] PROGMEM =
{
    {0x20,0x50,0xA8,0x20,0x20,0x20,0x00,0x00}, /* Up Arrow */
};

#define PBUF_SIZE 32
static uint8_t pbuf[PBUF_SIZE];
/*
static uint8_t pbuf_head = 0;
static uint8_t pbuf_tail = 0;
*/

uint8_t pbuf_front = 0;
uint8_t pbuf_rear = 0;

static uint8_t pbuf_size(void) { return pbuf_rear - pbuf_front; }
static bool pbuf_empty(void) { return pbuf_front == pbuf_rear; }
static bool pbuf_full(void) { return pbuf_size() == PBUF_SIZE; }
static void pbuf_clear(void) {pbuf_front = pbuf_rear = 0; }

static uint8_t pbuf_mask(uint8_t val)
{
    return val & (PBUF_SIZE - 1);
}

static void pbuf_add_rear(uint8_t val)
{
    if (!pbuf_full())
        pbuf[pbuf_mask(pbuf_rear++)] = val;
}

static uint8_t pbuf_remove_front(void)
{
    uint8_t val = 0;
    if (!pbuf_empty())
        val = pbuf[pbuf_mask(pbuf_front++)];

    return val;
}

static uint8_t pbuf_remove_rear(void)
{
    uint8_t val = 0;
    if (!pbuf_empty())
        val = pbuf[pbuf_mask(pbuf_rear--)];

    return val;
}

static uint8_t pbuf_peek_n(uint8_t n)
{
    uint8_t val = 0;
    if (!pbuf_empty())
        val = pbuf[pbuf_mask(pbuf_front + n)];

    return val;
}

uint8_t map_usb_hid(uint8_t hid_code, bool shift)
{
    uint8_t ascii = 0x00;
    if (hid_code >= 0x04 && hid_code <= 0x1D) /* A through Z */
    {
        ascii = (hid_code - 0x04) + 0x41;
        if (shift)
            ascii += 0x20;
    }
    else if (hid_code >=0x1E  && hid_code <= 0x27) /* Symbols */
    {
        if (shift)
            ascii = asciimap_numbers_shift[hid_code - 0x1E];
        else if (hid_code == 0x27)
            ascii = '0';
        else
            ascii = (hid_code - 0x1E) + 0x31;
    }
    else if (hid_code >=0x2C  && hid_code <= 0x39) /* Number row */
    {
        if (shift)
            ascii = asciimap_symbols_shift[hid_code - 0x2C];
        else
            ascii = asciimap_symbols[hid_code - 0x2C];
    }
    else
        ascii = 0xFF;

    return ascii;
}

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

void display_glyph(const uint8_t* glyph, uint8_t x, uint8_t y, bool write_empty)
{
    uint8_t i,j;
    uint8_t data;

    for (j = 0; j < CHAR_HEIGHT; j++)
    {
        data = pgm_read_byte(&(glyph[j]));
        for (i = 8 - CHAR_WIDTH; i < 8; i++)
        {
            if ((data & (1 << i)) != 0)
                h2525_set_pixel(x + i, y + j);
            else if (write_empty)
                h2525_clear_pixel(x + i, y + j);
        }
    }
}


#define ALPHA_X 4
void display_char(uint8_t ascii, uint8_t index, uint8_t y, bool write_empty)
{
    const uint8_t* data = font[ascii];
    uint8_t x = ALPHA_X + (index * CHAR_WIDTH);
    display_glyph(data, x, y, write_empty);
}


static void update_alpha(void)
{
    uint8_t count = pbuf_size();
    display_char(count/10 + '0', 2, 10, true);
    display_char(count%10 + '0', 3, 10, true);
    for (int i = 0; i < count; i++)
    {
        uint8_t ascii;
        ascii = pbuf_peek_n(i);
        display_char(ascii, i, 0, true);
    }
    if (count == PBUF_SIZE)
        display_char('_', count - 1, 0, false);
    else
    {
        display_char('_', count, 0, true);
        for( int i = count + 1; i < PBUF_SIZE; i++)
            display_char(' ', i, 0, true);
    }

}

static void update_special(uint8_t hid_code)
{
    switch (hid_code)
    {
        case KEY_BACKSPACE: /* Backspace */
            pbuf_remove_rear();
            break;
        case KEY_ENTER:
            pbuf_clear();
            break;
        default:
            break;
    }
}


void display_init(void)
{
    h2525_init();
    h2525_start();
}

void display_clear(void)
{
    h2525_clear();
}

void display_toggle_pwd_blanking(void)
{
    pwd_blanking_on = !pwd_blanking_on;

}


void display_on_hid_code(uint8_t hid_code)
{
    uint8_t ascii = 0;

    if (pwd_blanking_on) {
        if (hid_code == KEY_ENTER)
            pwd_blanking_on = false;
        else
            ascii = '*';
    }

    if (ascii == 0)
        ascii = map_usb_hid(hid_code, false);

    if (ascii == 0xFF)
        update_special(hid_code);
    else
    {
        if (pbuf_full())
            pbuf_remove_front();
        pbuf_add_rear(ascii);
    }
    update_alpha();
}
