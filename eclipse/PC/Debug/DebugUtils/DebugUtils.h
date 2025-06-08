/*
 * DebugUtils.h
 *
 *  Created on: 19 Jan 2025
 *      Author: rw123
 */

#ifndef DEBUGUTILS_H_
#define DEBUGUTILS_H_

#include <stdint.h>

typedef struct{
  char* regName;
  uint8_t* reg;
} debugUtils_regPrintItem;

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------------
// prints specified registers (name: value) to the string. Returns a pointer to the string.
char* debugUtils_regSprint(debugUtils_regPrintItem registers[], uint8_t count, char* str);
//--------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* DEBUGUTILS_H_ */
