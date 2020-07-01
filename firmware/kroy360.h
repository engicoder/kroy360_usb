#ifndef KROY360_H
#define KROY360_H

#include "quantum.h"

void matrix_init_user(void);

enum kroy360_keycodes {
	PWD_TOG = SAFE_RANGE,
};


#define KEYMAP( \
    K24, K25, K1C, K1D, K14, K15, K0C, K0D, K04, K05,      K44, \
                                                           K45, \
    K2D, K26, K27, K1E, K1F, K16, K17, K0E, K0F, K06, K07, K46, K47, K4E, \
    K2C, K2F, K21, K18, K19, K10, K11, K08, K09, K49, K01, K40, K4F,         K4C, K4D, K34, \
    K2E, K20, K2A, K22, K1A, K12, K0A, K02, K42, K4A, K32, K41, K36,         K37, K3E, K35, \
    K29, K2B, K23, K1B, K13, K0B, K03, K43, K4B, K33, K3A, K48,              K3F, K3D, K3C, \
         K28,                     K3B \
) { \
    { KC_NO, K01  , K02  , K03  , K04  , K05  , K06  , K07  , K08  , K09  , K0A  , K0B  , K0C  , K0D  , K0E  , K0F  , }, \
    { K10  , K11  , K12  , K13  , K14  , K15  , K16  , K17  , K18  , K19  , K1A  , K1B  , K1C  , K1D  , K1E  , K1F  , }, \
    { K20  , K21  , K22  , K23  , K24  , K25  , K26  , K27  , K28  , K29  , K2A  , K2B  , K2C  , K2D  , K2E  , K2F  , }, \
    { KC_NO, KC_NO, K32  , K33  , K34  , K35  , K36  , K37  , KC_NO, KC_NO, K3A  , K3B  , K3C  , K3D  , K3E  , K3F  , }, \
    { K40  , K41  , K42  , K43  , K44  , K45  , K46  , K47  , K48  , K49  , K4A  , K4B  , K4C  , K4D  , K4E  , K4F  , }, \
}

#endif
