/*

Serial monitor Ack frame: TST16A452
*/
#include "env.h"

// #include <avr/io.h>
// #include "wiring_private.h"

// #include <stdio.h>
#include <stdarg.h>
// #include "uart_wrapper.hpp"
#include "timer0.h"
#include "hw_gpio.h"
#include "swTimer.h"
#include "Writer.hpp"
#include "Reader.hpp"
// #include "System0.hpp"
#include "Socket.hpp"

void toggleBuiltInLed();
bool debugSockInstantHandler(SerLink::Frame &rxFrame, uint16_t* dataLen, char* data);

static uint16_t startTick;
static bool g_ledState = false;
static char rxFrameFromReader0Buffer[UART_BUFF_LEN];
static SerLink::Frame rxFrameFromReader0(rxFrameFromReader0Buffer);  // received frame from reader0
static char txFrameBuffer[UART_BUFF_LEN];

//-------------------------------------------------
// reader0 & writer0

char readerRxBuffer[UART_BUFF_LEN];
char readerAckBuffer[UART_BUFF_LEN];
char writerTxBuffer[UART_BUFF_LEN];

char writerTxFrameBuffer[UART_BUFF_LEN];
char writerAckFrameBuffer[UART_BUFF_LEN];
SerLink::Frame writerTxFrame(writerTxFrameBuffer);
SerLink::Frame writerAckFrame(writerAckFrameBuffer);

char readerRxFrameBuffer[UART_BUFF_LEN];
char readerAckFrameBuffer[UART_BUFF_LEN];
SerLink::Frame readerRxFrame(readerRxFrameBuffer);
SerLink::Frame readerAckFrame(readerAckFrameBuffer);

SerLink::Writer writer0(WRITER_CONFIG__WRITER0_ID, writerTxBuffer,
    UART_BUFF_LEN, &writerTxFrame, &writerAckFrame);

SerLink::Reader reader0(READER_CONFIG__READER0_ID, readerRxBuffer, readerAckBuffer,
    UART_BUFF_LEN, &readerRxFrame, &readerAckFrame, &writer0);

//----------------------------------------------
//-----------------------
// ledSocket
char ledSocketRxFrameBuffer[UART_BUFF_LEN];
char ledSocketTxFrameBuffer[UART_BUFF_LEN];

SerLink::Frame ledSocketRxFrame(ledSocketRxFrameBuffer);
SerLink::Frame ledSocketTxFrame(ledSocketTxFrameBuffer);

SerLink::Socket ledSocket(&writer0, &reader0, "LED01", &ledSocketRxFrame, &ledSocketTxFrame);

#define LED_SOCKET_RX_DATA_LEN 10
char ledSocketData[LED_SOCKET_RX_DATA_LEN] = {0};
uint16_t ledSocketRxDataLen;
//-----------------------
// echo socket
char echoSocketRxFrameBuffer[UART_BUFF_LEN];
char echoSocketTxFrameBuffer[UART_BUFF_LEN];

SerLink::Frame echoSocketRxFrame(ledSocketRxFrameBuffer);
SerLink::Frame echoSocketTxFrame(ledSocketTxFrameBuffer);

SerLink::Socket echoSocket(&writer0, &reader0, "ECHO1", &echoSocketRxFrame, &echoSocketTxFrame);

//-----------------------
// debug socket
char debugSocketRxFrameBuffer[UART_BUFF_LEN];
char debugSocketTxFrameBuffer[UART_BUFF_LEN];

SerLink::Frame debugSocketRxFrame(debugSocketRxFrameBuffer);
SerLink::Frame debugSocketTxFrame(debugSocketTxFrameBuffer);

SerLink::Socket debugSocket(&writer0, &reader0, "DBG01", &debugSocketRxFrame, &debugSocketTxFrame, &debugSockInstantHandler);

//-----------------------
// general purpose socket data buffers

#define SOCKET_RX_DATA_LEN 12
char socketRxData[SOCKET_RX_DATA_LEN] = {0};
char socketTxData[SOCKET_RX_DATA_LEN] = {0};
uint16_t socketRxDataLen;
//-----------------------

//SerLink::Frame txFrame("TST16", SerLink::Frame::TYPE_TRANSMISSION, 452, 4, "abcd");
//static SerLink::Frame txFrame("TST16", SerLink::Frame::TYPE_TRANSMISSION, 452, txFrameBuffer, 4, "abcd");

uint8_t count = 0;
static char txData[4];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(7, OUTPUT);

  //uart_init(rxBuffer, UART_BUFF_LEN);

  reader0.init();

  timer0_init();

  swTimer_tickReset(&startTick);

  //strncpy(ledSocketPayload, "a1", 2);

  // Pin B5 (Pin 13)
  gpio_setPinDirection(GPIO_REG__PORTB, 5, GPIO_PIN_DIRECTION__OUT);
  //gpio_setPinDirection(GPIO_REG__PORTB, 5, GPIO_PIN_DIRECTION__IN);

  gpio_setPinLow(GPIO_REG__PORTB, 5);

  //--------------------------
  // Pin B4 (Pin 12)
  gpio_setPinDirection(GPIO_REG__PORTB, 4, GPIO_PIN_DIRECTION__IN);

  sprintf(txData, "AAA", nullptr);
} // end setup()


void loop() {

  static bool send = false;
  static bool frameSent = false;
  
  uint16_t txFrameCount = 0;
  

  cli();
  if(swTimer_tickCheckTimeout(&startTick, 3000))
  {
    //sprintf(txData, "%03d", count++);
    //ledSocket.sendData(txData, 3, true);
    //frameSent = true;

    //toggleBuiltInLed();
  }
  sei();

  if(frameSent)
  {
    uint8_t status = ledSocket.getAndClearSendStatus();
    if(status == SerLink::Writer::STATUS_BUSY)
    {
      // do nothing
    }
    else
    {
      frameSent = false;
      sprintf(txData, "%02d", count++);
      if(count > 99){ count = 0; }
      
      if(status == SerLink::Writer::STATUS_TIMEOUT)
      {
        txData[2] = 'T';
        //strncpy(ledSocketPayload, "TO", 2);
      } else if(status == SerLink::Writer::STATUS_OK)
      {
        //strncpy(ledSocketPayload, "OK", 2); // LED01A006
        txData[2] = 'K';
      } else{
        //strncpy(ledSocketPayload, "XX", 2);
        txData[2] = 'X';
      }
      
    }
  }

  // special case (does not send ack back): NOACKT234003asd

  if(ledSocket.getRxData(socketRxData, &socketRxDataLen)) // LED01T8050011  LED01T8050010
  {
    if(socketRxData[0] == '1')
    {
      gpio_setPinHigh(GPIO_REG__PORTB, 5);
    }
    else if(socketRxData[0] == '0')
    {
      gpio_setPinLow(GPIO_REG__PORTB, 5);
    }
    else{ /* do nothing */ }
    memset(socketRxData, 0, SOCKET_RX_DATA_LEN);
  }

  if(echoSocket.getRxData(socketRxData, &socketRxDataLen)) // ECHO1T076004abcd LED01T805003abc DBG01T156003ASD
  {
    sprintf(socketTxData, "echo %s\0", socketRxData);
    echoSocket.sendData(socketTxData, socketRxDataLen + 5, false);
    memset(socketRxData, 0, SOCKET_RX_DATA_LEN);
  }

  // MUST check socket for rx data in order to clear flags - even if we are going to ignore the data
  if(debugSocket.getRxData(socketRxData, &socketRxDataLen)) // ECHO1T076004abcd LED01T805003abc DBG01T156003ASD DBG01T156003aSD
  {
    // sprintf(socketTxData, "dbg %s\0", socketRxData);
    // echoSocket.sendData(socketTxData, socketRxDataLen + 4, false);
    // memset(socketRxData, 0, SOCKET_RX_DATA_LEN);
  }
  reader0.clearRxFlag();
  writer0.run();
  reader0.run();

} // end loop()

bool debugSockInstantHandler(SerLink::Frame &rxFrame, uint16_t* dataLen, char* data)
{
  if(rxFrame.buffer[0] == 'A')
  {
    memset(data, 0, 10); // clear outgoing buffer
    strcpy(data, "ABCD\0");
    *dataLen = 4;
    return true;
  }
  else
  {
    return false;
  }
}

void toggleBuiltInLed()
{
  if(g_ledState){
    digitalWrite(LED_BUILTIN, HIGH);
    //PORTD |= (1 << PD7);
    }
  else {
    digitalWrite(LED_BUILTIN, LOW);
    //PORTD &= ~(1 << PD7); //0x7f;      
    }
  g_ledState = !g_ledState;
}