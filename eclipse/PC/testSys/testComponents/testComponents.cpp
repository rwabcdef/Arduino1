/*
 * testComponents.cpp
 *
 *  Created on: 13 Dec 2024
 *      Author: rw123
 */

#include "testComponents.hpp"


char tc_debugStr[ENV_DEBUG_STR_LEN] = {0};
//-------------------------------------------------
// uart Tx ISR
char tc_uartTxFrame[128] = {0};
uint8_t tc_uartTxCharIndex = 0;


//-------------------------------------------------


Event tc_uartRx(tc_uartRxHandler);  // **** MUST be static - otherwise stack overflow

InterruptSchedule* tc_pUartRxInterrupt;

//char tc_rxFrameFromReader0Buffer[UART_BUFF_LEN];
//SerLink::Frame tc_rxFrameFromReader0(tc_rxFrameFromReader0Buffer);  // received frame from reader0

// The uart hardware bit: UDRIE0, is set in uart_write().
// This function is then called, which sets up a single interrupt,
// which corresponds to the uart data register becoming empty.
// The ISR then loads the uart data register with the first character.
void tc_Set_UDRIE0_CallBack()
{
  debugPrint->writeLine("Set_UDRIE0_CallBack", DebugPrint_defs::UartTx);

  g_enable_USART_UDRE_ISR = true;

  std::list<uint32_t>* lst = new std::list<uint32_t>();
  lst->push_back((uint32_t) (simClk->getCurrent() + 200));

  InterruptSchedule* nextCharRun = new InterruptSchedule((InterruptRoutine)&tc_uartTxRun, lst);
  interruptRunner->RegisterInterruptSchedule(nextCharRun);
}

void tc_uartTxRun()
{
  cli();
  RUN_ISR(UART_ISR_TX); // Run the Uart tx interrupt routine

  char c = UDR0;

  tc_uartTxFrame[tc_uartTxCharIndex++] = c;

  static char s[32];
  memset(s, 0, 32);

  if(c == '\n')
  {
    strcpy(s, "uartTx: endl");
    sprintf(tc_debugStr, "uart Tx frame: %s", tc_uartTxFrame);
    //debugPrint->writeLine(debugStr, DebugPrint_defs::UartTx);
    memset(tc_debugStr, 0, ENV_DEBUG_STR_LEN);
    tc_uartTxCharIndex = 0;
  }
  else
  {
    sprintf(s, "uartTx: %c", c);
  }
  debugPrint->writeLine(s, DebugPrint_defs::UartTx);

  if(g_enable_USART_UDRE_ISR)
  {
    // Initialise another ISR trigger
    std::list<uint32_t>* lst = new std::list<uint32_t>();
    lst->push_back((uint32_t) (simClk->getCurrent() + 500));

    InterruptSchedule* nextCharRun = new InterruptSchedule((InterruptRoutine)&tc_uartTxRun, lst);
    interruptRunner->RegisterInterruptSchedule(nextCharRun);
  }
  sei();
}
void tc_Clr_UDRIE0_CallBack()
{
  debugPrint->writeLine("Clr_UDRIE0_CallBack", DebugPrint_defs::UartTx);
  g_enable_USART_UDRE_ISR = false;
}
//---------------------------------------------------------------
void tc_uartRxHandler(void* pData)
{
  uint64_t n = (uint64_t)pData;
  char c = (char)n;

  UDR0 = c;    // load ATmega328 Uart data register with the received char


  cli();
  RUN_ISR(UART_ISR_RX); // Run the Uart rx interrupt routine
  sei();

  static char s[32];
  memset(s, 0, 32);
  if(c == '\n')
  {
    sprintf(s, "rx: endl", 0);
  }
  else
  {
    sprintf(s, "rx: %c", c);
  }

  debugPrint->writeLine(s, DebugPrint_defs::UartRx);
  sei();
}

//---------------------------------------------------------------
