/*
 * testSys.hpp
 *
 *  Created on: 26 Jun 2023
 *      Author: rw123
 */
#include <stdint.h>

#ifndef TESTSYS_HPP_
#define TESTSYS_HPP_

#define TESTSYS_TEST_MODE_NONE 0
#define TESTSYS_TEST_MODE_UNIT 1

extern uint8_t TESTSYS_mode;

void getIntThreadId_ISR();

void InitTimestamp();

void initDebug();

void initSys(uint32_t simTicks);

void initRun(uint32_t simTicks);

void endRun();

void endSys();

void setUseRealTimeInTimestamp(bool b);

void MakeTimestamp(char* pTimeStamp);

void timer0_ISR();


#endif /* TESTSYS_HPP_ */
