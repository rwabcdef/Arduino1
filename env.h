/*
 * env.h
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#ifndef ENV_H_
#define ENV_H_

#include "env_config.h"

#if defined(ENV_CONFIG__SYSTEM_PC)

#include<stdbool.h>
#include<ATmega328.hpp>

#elif defined(ENV_CONFIG__SYSTEM_ARDUINO_UNO_R3)
//#include <Arduino.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#endif

#endif /* ENV_H_ */
