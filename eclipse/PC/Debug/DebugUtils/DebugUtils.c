/*
 * DebugUtils.c
 *
 *  Created on: 19 Jan 2025
 *      Author: rw123
 */

#include "DebugUtils.h"
#include <stdio.h>

//--------------------------------------------------------------------------------
char* debugUtils_regSprint(debugUtils_regPrintItem registers[], uint8_t count, char* str)
{
  uint8_t i = 0, end = 0;

  for(i=0; i<count; i++)
  {
    end += sprintf(&str[end], "%s:%02x ", registers[i].regName, *registers[i].reg);
  }
  str[end] = 0;
  return str;
}
//--------------------------------------------------------------------------------
