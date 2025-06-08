/*
 * ButtonModuleDevelop.cpp
 *
 *  Created on: 26 May 2025
 *      Author: rw123
 */
#include "ButtonModuleDevelop.hpp"
#include "ButtonModule.hpp"
#include "timer0.h"

namespace HardwareModule::Std{

void ButtonModuleDevelop::run1()
{
  g_timer0_ISR = 0;

  ButtonModule* buttonModule = new ButtonModule(2, 3, false);
}

};

