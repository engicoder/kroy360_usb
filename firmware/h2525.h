#ifndef H2525_H
#define H2525_H

void h2525_init(void);

void h2525_start(void);

void h2525_clear(void);
void h2525_set_pixel(uint8_t x, uint8_t y);
void h2525_clear_pixel(uint8_t x, uint8_t y);

void h2525_clear_region(uint8_t start_x, uint8_t start_y, uint8_t width, uint8_t height);

#endif
