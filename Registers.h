#ifndef REGISTERS_H
#define REGISTERS_H
// Registers.h
// Created on: 25 Oct 2025
// Created by: Rob Woodhouse

#include <stdint.h>
#include "env.h"

#if defined(ENV_CONFIG__SYSTEM_ARDUINO_UNO_R3)

#ifdef __cplusplus
extern "C" {
#endif

uint8_t readPortB(char* str);

#ifdef __cplusplus
}
#endif

#endif

#endif // REGISTERS_H