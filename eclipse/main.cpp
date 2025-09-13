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

/*
 ******* IMPORTANT

 To sync files from the arduino IDE project (i.e. the actual embedded code)
 and the eclipse source code, run:
 python .\eclipse_sync.py

 from:
C:\Users\rw123\OneDrive\Documents\Rob\software\Embedded\Arduino\repo\Arduino1\Arduino1

 */

//DebugPrint* debugPrint;

int main()
{
  printf("Main start 4\n");

  HardMod::Std::ButtonModuleDevelop::buttonEvent1();

  return 0;
}

