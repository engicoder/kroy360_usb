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

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "kroy360_if.h"

#ifndef DEBOUNCE
#   define DEBOUNCE	0
#endif

static matrix_row_t matrix[MATRIX_ROWS];

static uint8_t is_down[0x50] = {0};

void matrix_make(uint8_t code);
void matrix_break(uint8_t code);

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_setup(void)
{
    kroy360_if_init();
}

void matrix_init(void)
{
    debug_config.enable = true;

    for (int r = 0; r < MATRIX_ROWS; r++)
        matrix[r] = 0;
}


#define ROW(pos) (pos >> 4)
#define COL(pos) (pos & 0xF)

uint8_t matrix_scan(void)
{
    if(kroy360_if_has_data())
    {
        uint8_t data = kroy360_if_recv();
        uint8_t code = kroy360_if_scan_code(data);
        if (kroy360_if_pressed(data))
            matrix_break(code);
        else
            matrix_make(code);
    }

    return 1;
}



void matrix_break(uint8_t code)
{
    uint8_t row;
    uint8_t col;

    xprintf("break %X\n", code);
    row = ROW(code);
    col = COL(code);
    matrix[row] &= ~(1 << col);
    is_down[code] = false;
}

void matrix_make(uint8_t code)
{
    uint8_t row;
    uint8_t col;

    xprintf("make %X\n", code);
    row = ROW(code);
    col = COL(code);
    matrix[row] |= 1 << col;
    is_down[code] = true;
}


inline
matrix_row_t matrix_get_row(uint8_t row)
{
    matrix_row_t matrix_row = matrix[row];
    return matrix_row;
}

void matrix_print(void)
{
    print("\nr/c 01234567\n");
    for (uint8_t row = 0; row < matrix_rows(); row++) {
        xprintf("%02X: %08b\n", row, bitrev(matrix_get_row(row)));
    }
}

