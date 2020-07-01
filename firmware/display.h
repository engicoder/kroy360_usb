#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

void display_init(void);
void display_on_hid_code(uint8_t hid_code);
void display_toggle_pwd_blanking(void);
void display_clear(void);
void display_char(uint8_t ascii, uint8_t index, uint8_t y, bool write_empty);
void display_glyph(const uint8_t* glyph, uint8_t x, uint8_t y, bool write_empty);

#endif
