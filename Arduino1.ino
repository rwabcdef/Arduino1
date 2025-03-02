/*

Serial monitor Ack frame: TST16A452
*/
#include "env.h"

// #include <avr/io.h>
// #include "wiring_private.h"

// #include <stdio.h>
// #include <stdarg.h>
// #include "uart_wrapper.hpp"
#include "timer0.h"
#include "swTimer.h"
#include "Writer.hpp"
#include "Reader.hpp"
// #include "System0.hpp"
#include "Socket.hpp"

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

char ledSocketPayload[2] = {0};

#define LED_SOCKET_RX_DATA_LEN 10
char ledSocketRxData[LED_SOCKET_RX_DATA_LEN] = {0};
uint16_t ledSocketRxDataLen;
//-----------------------

//SerLink::Frame txFrame("TST16", SerLink::Frame::TYPE_TRANSMISSION, 452, 4, "abcd");
static SerLink::Frame txFrame("TST16", SerLink::Frame::TYPE_TRANSMISSION, 452, txFrameBuffer, 4, "abcd");


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(7, OUTPUT);

  //uart_init(rxBuffer, UART_BUFF_LEN);

  reader0.init();

  timer0_init();

  swTimer_tickReset(&startTick);

  strncpy(ledSocketPayload, "a1", 2);
}

void loop() {

  static bool send = false;
  static bool txFrameSent = false;
  static uint8_t count = 0;
  static uint16_t txFrameCount = 0;

  void toggleBuiltInLed();
  
  cli();
  if(swTimer_tickCheckTimeout(&startTick, 1000))
  {
    

    //writer0.sendFrame(txFrame, false);
    //toggleBuiltInLed();

    count++;
    if((count % 4) == 0)
    {
      send = true;
    }

    //toggleBuiltInLed();
  }
  sei();

  if(send)
  {
    send = false;
    // writer0.sendFrame(&txFrame);

    
    if(true == ledSocket.sendData(ledSocketPayload, 2, true))   // use socket to send frame LED01A
    {
      //toggleBuiltInLed();
    }
    txFrameSent = true;

    //toggleBuiltInLed();
  }

  if(txFrameSent)
  {
    //uint8_t status = writer0.getStatus();
    uint8_t status = ledSocket.getAndClearSendStatus();
    if(status == SerLink::Writer::STATUS_BUSY)
    {
      // do nothing
    }
    else
    {
      txFrameSent = false;
      if(status == SerLink::Writer::STATUS_TIMEOUT)
      {
        txFrame.buffer[3] = 'T';
        strncpy(ledSocketPayload, "TO", 2);
      } else if(status == SerLink::Writer::STATUS_OK)
      {
        strncpy(ledSocketPayload, "OK", 2);
        txFrame.buffer[3] = 'K';
      } else{
        strncpy(ledSocketPayload, "XX", 2);
        txFrame.buffer[3] = 'X';
      }
      txFrameCount++;
      SerLink::Frame::int3dToStr(txFrameCount, txFrame.buffer);
    }
  }

  //if(reader0.getRxFrameProtocol(&rxFrameFromReader0, "TST03")) // TST03T287002ab
  if(ledSocket.getRxData(ledSocketRxData, &ledSocketRxDataLen)) // LED01T805003abc
  {
    toggleBuiltInLed();
  }

  writer0.run();
  reader0.run();
  //transport0.run();
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