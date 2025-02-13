/*
 * EmbUtils.hpp
 *
 *  Created on: 16 Jul 2023
 *      Author: rw123
 */

#ifndef EMBUTILS_EMBUTILS_H_
#define EMBUTILS_EMBUTILS_H_

#include<stdint.h>

#define BITMASK_BIT_0 0x01
#define BITMASK_BIT_1 0x02
#define BITMASK_BIT_2 0x04
#define BITMASK_BIT_3 0x08
#define BITMASK_BIT_4 0x10
#define BITMASK_BIT_5 0x20
#define BITMASK_BIT_6 0x40
#define BITMASK_BIT_7 0x80

#define INV_BITMASK_BIT_0 0xFE
#define INV_BITMASK_BIT_1 0xFD
#define INV_BITMASK_BIT_2 0xFB
#define INV_BITMASK_BIT_3 0xF7
#define INV_BITMASK_BIT_4 0xEF
#define INV_BITMASK_BIT_5 0xDF
#define INV_BITMASK_BIT_6 0xBF
#define INV_BITMASK_BIT_7 0x7F

#define CHECK_BIT(reg, bit) (BITMASK_BIT[bit] & reg)
#define SET_BIT(reg, bit) (reg |= BITMASK_BIT[bit])
#define CLR_BIT(reg, bit) (reg &= INV_BITMASK_BIT[bit])
#define TOGGLE_BIT(reg, bit) if(CHECK_BIT(reg, bit)){CLR_BIT(reg, bit);}else{SET_BIT(reg, bit);}

#define sbi(reg, bit) SET_BIT(reg, bit)
#define cbi(reg, bit) CLR_BIT(reg, bit)

extern const uint8_t BITMASK_BIT[];
extern const uint8_t INV_BITMASK_BIT[];

//--------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// Appends the dest string with the specified register name and value, e.g.:
// PORTD: 0xc5
void appendStringRegister(char* dest, uint8_t startIndex, char* regName, uint8_t* reg);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------------------------------------

#endif /* EMBUTILS_EMBUTILS_H_ */
