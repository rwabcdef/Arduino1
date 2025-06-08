/*
 * EmbUtils.cpp
 *
 *  Created on: 16 Jul 2023
 *      Author: rw123
 */

#include "EmbUtils.h"

#include <stdio.h>

const uint8_t BITMASK_BIT[] = {BITMASK_BIT_0, BITMASK_BIT_1, BITMASK_BIT_2,
BITMASK_BIT_3, BITMASK_BIT_4, BITMASK_BIT_5, BITMASK_BIT_6, BITMASK_BIT_7};

const uint8_t INV_BITMASK_BIT[] = {INV_BITMASK_BIT_0, INV_BITMASK_BIT_1, INV_BITMASK_BIT_2,
INV_BITMASK_BIT_3, INV_BITMASK_BIT_4, INV_BITMASK_BIT_5, INV_BITMASK_BIT_6, INV_BITMASK_BIT_7};


void appendStringRegister(char* dest, uint8_t startIndex, char* regName, uint8_t* reg)
{
//  uint8_t index = startIndex;
//  uint8_t j = 0;
//
//  while(regName[j] != 0)
//  {
//    dest[index++] = regName[j];
//    j++;
//  }
//
//  dest[index++] = ':';
//  dest[index++] = ' ';
//  dest[index++] = '0';
//  dest[index++] = 'x';

  sprintf(&dest[startIndex], "%s: 0x%x\0", (const char*) regName, *reg);
}
