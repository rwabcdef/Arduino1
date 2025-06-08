/*
 * DebugPrintFromC.cpp
 *
 *  Created on: 5 Oct 2024
 *      Author: rw123
 */

#include "DebugPrintFromC.hpp"
#include "Global.hpp"
#include "env.hpp"

void debugWrite(char* str, DebugLevels level) // , DebugPrint_defs::Levels level
{
  debugPrint->writeLine(str, DebugPrint_defs::Levels(level));
}

