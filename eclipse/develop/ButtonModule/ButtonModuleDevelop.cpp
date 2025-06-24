/*
 * ButtonModuleDevelop.cpp
 *
 *  Created on: 26 May 2025
 *      Author: rw123
 */
#include "ButtonModuleDevelop.hpp"
#include "ButtonModule.hpp"
#include "timer0.h"
#include "ButtonEvent.hpp"
#include <stdio.h>

//----------------------------------------------------------
HardMod::ButtonEvent buttonEvent0;
//----------------------------------------------------------


namespace HardMod::Std{

void ButtonModuleDevelop::run1()
{
  g_timer0_ISR = 0;

  ButtonModule* buttonModule = new ButtonModule(2, 3, false);
}

void ButtonModuleDevelop::buttonEvent1()
{
  char s[8];

  buttonEvent0.setAction(BUTTONEVENT__RELEASED);
  buttonEvent0.setPressDuration(15);

  buttonEvent0.serialise(s);
  printf("s: %s\n", s);
}

};

