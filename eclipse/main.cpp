/*
 * main.cpp
 *
 *  Created on: 24 Mar 2025
 *      Author: rw123
 */

#include "env_config.h"
#include <stdio.h>
#include "Global.hpp"
#include "ButtonModuleDevelop.hpp"

//DebugPrint* debugPrint;

int main()
{
  printf("Main start 4\n");

  HardMod::Std::ButtonModuleDevelop::buttonEvent1();

  return 0;
}

