/*
 * testComponents.hpp
 *
 *  Created on: 13 Dec 2024
 *      Author: rw123
 */

#ifndef TESTCOMPONENTS_HPP_
#define TESTCOMPONENTS_HPP_

#include "env.hpp"
#include "Global.hpp"
#include "Event.hpp"
#include "cute.h"
#include "uart_wrapper.hpp"
#include "Frame.hpp"
#include "Reader_config.hpp"
#include "Reader.hpp"
#include "Writer.hpp"
#include "InterruptSchedule.hpp"
#include <stdlib.h>
#include <string.h>

void tc_uartRxHandler(void* pData);
void tc_Set_UDRIE0_CallBack();
void tc_Clr_UDRIE0_CallBack();
void tc_uartTxRun();

extern char tc_debugStr[];
//-------------------------------------------------
// uart Tx ISR
extern char tc_uartTxFrame[];
extern uint8_t tc_uartTxCharIndex;


//-------------------------------------------------


extern Event tc_uartRx;  // **** MUST be static - otherwise stack overflow

extern InterruptSchedule* tc_pUartRxInterrupt;

extern char tc_rxFrameFromReader0Buffer[];
extern SerLink::Frame tc_rxFrameFromReader0;

#endif /* TESTCOMPONENTS_HPP_ */
