/*
 * ATmega328Debug.c
 *
 *  Created on: 19 Jan 2025
 *      Author: rw123
 */

#include "ATmega328Debug.h"
#include "DebugUtils.h"

const debugUtils_regPrintItem ATmega328Debug_portD[] = {{"PIND\0", &PIND},{"DDRD\0", &DDRD},{"PORTD\0", &PORTD}};

//--------------------------------------------------------------------------------
char* ATmega328Debug_sprintPortD(char* str)
{
  return debugUtils_regSprint(ATmega328Debug_portD, 3, str);
}
//--------------------------------------------------------------------------------

