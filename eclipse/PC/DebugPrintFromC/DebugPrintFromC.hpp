/*
 * DebugPrintFromC.hpp
 *
 *  Created on: 5 Oct 2024
 *      Author: rw123
 */

//#include "env.hpp"

#ifndef DEBUGPRINTFROMC_HPP_
#define DEBUGPRINTFROMC_HPP_

typedef enum
{
  Gpio = 13
}DebugLevels;

#ifdef __cplusplus
extern "C" {
#endif

void debugWrite(char* str, DebugLevels level); //, DebugPrint_defs::Levels level);

#ifdef __cplusplus
}
#endif

#endif /* DEBUGPRINTFROMC_HPP_ */
