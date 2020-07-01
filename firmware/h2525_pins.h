#ifndef H2525_PINS_H
#define H2525_PINS_H


#define H2525_FLM   D,PD6
#define H2525_M     D,PD1
#define H2525_CL1   D,PD2
#define H2525_CL2   D,PD5
#define H2525_D1    D,PD3
/*
#define H2525_FLM   F,4
#define H2525_M     F,5
#define H2525_CL1   F,6
#define H2525_CL2   F,7
#define H2525_D1    B,1
*/
#define PIN_CONFIG_OUT_LOW2(port, pin) \
    do { \
        PORT##port &= ~(1 << pin); \
        DDR##port |= (1 << pin); \
    } while(0)
#define PIN_CONFIG_OUT_LOW(...) PIN_CONFIG_OUT_LOW2(__VA_ARGS__)

#define PIN_CONFIG_OUT_HIGH2(port,pin) \
    do { \
        PORT##port |= (1 << pin); \
        DDR##port |= (1 << pin); \
    } while(0)
#define PIN_CONFIG_OUT_HIGH(...) PIN_CONFIG_OUT_HIGH2(__VA_ARGS__)

#define PIN_LOW2(port, pin) \
    do { \
        PORT##port &= ~(1 << pin); \
    } while(0)
#define PIN_LOW(...) PIN_LOW2(__VA_ARGS__)

#define PIN_HIGH2(port, pin) \
    do { \
        PORT##port |= (1 << pin); \
    } while(0)
#define PIN_HIGH(...) PIN_HIGH2(__VA_ARGS__)

#define PIN_TOGGLE2(port, pin) \
    do { \
        PORT##port ^= (1 << pin); \
    } while(0)
#define PIN_TOGGLE(...) PIN_TOGGLE2(__VA_ARGS__)


#endif
