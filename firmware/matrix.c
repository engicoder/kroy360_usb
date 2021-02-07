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
#include "debounce.h"
#include "kroy360_if.h"

#ifndef DEBOUNCE
#   define DEBOUNCE	0
#endif

static matrix_row_t raw_matrix[MATRIX_ROWS];  // raw values
static matrix_row_t matrix[MATRIX_ROWS];      // debounced values

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

void matrix_init(void)
{
    debug_config.enable = true;

    for (int r = 0; r < MATRIX_ROWS; r++) {
        raw_matrix[r] = 0;
        matrix[r] = 0;
    }

    kroy360_if_init();

    debounce_init(MATRIX_ROWS);
    matrix_init_quantum();  
}


#define ROW(pos) (pos >> 4)
#define COL(pos) (pos & 0xF)

uint8_t matrix_scan(void)
{
    bool changed = false;

    if(kroy360_if_has_data())
    {
        changed = true;
        uint8_t data = kroy360_if_recv();
        uint8_t code = kroy360_if_scan_code(data);
        if (kroy360_if_pressed(data))
            matrix_make(code);
        else
            matrix_break(code);
    }

    debounce(raw_matrix, matrix, MATRIX_ROWS, changed);

    matrix_scan_quantum();    

    return changed;
}



void matrix_break(uint8_t code)
{
    uint8_t row;
    uint8_t col;

    xprintf("break %02X\n", code);
    row = ROW(code);
    col = COL(code);
    raw_matrix[row] &= ~(1 << col);
}

void matrix_make(uint8_t code)
{
    uint8_t row;
    uint8_t col;

    xprintf("make %02X\n", code);
    row = ROW(code);
    col = COL(code);
    raw_matrix[row] |= 1 << col;
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

