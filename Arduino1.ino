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
#include "ButtonModule.hpp"
#include "HwModule.hpp"
#include "HardMod_EventQueue.hpp"

/*
Hardware Config

Green Led: Pin 13 (PB5)
Red Led: Pin 12 (PB4)
Button0: Pin 11 (PB3)
*/

void toggleBuiltInLed();
bool debugSockInstantHandler(SerLink::Frame &rxFrame, uint16_t* dataLen, char* data);

static uint16_t startTick;
static bool g_ledState = false;
static char rxFrameFromReader0Buffer[UART_BUFF_LEN];
static SerLink::Frame rxFrameFromReader0(rxFrameFromReader0Buffer);  // received frame from reader0
static char txFrameBuffer[UART_BUFF_LEN];

static uint16_t buttonStartTick;
static bool buttonListen = true;
static uint8_t buttonCount = 0;

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

SerLink::Frame echoSocketRxFrame(echoSocketRxFrameBuffer);
SerLink::Frame echoSocketTxFrame(echoSocketTxFrameBuffer);

SerLink::Socket echoSocket(&writer0, &reader0, "ECHO1", &echoSocketRxFrame, &echoSocketTxFrame);

//-----------------------
// debug socket
char debugSocketRxFrameBuffer[UART_BUFF_LEN];
char debugSocketTxFrameBuffer[UART_BUFF_LEN];

SerLink::Frame debugSocketRxFrame(debugSocketRxFrameBuffer);
SerLink::Frame debugSocketTxFrame(debugSocketTxFrameBuffer);

SerLink::Socket debugSocket(&writer0, &reader0, "DBG01", &debugSocketRxFrame, &debugSocketTxFrame, nullptr, nullptr, &debugSockInstantHandler);
//-----------------------
// button socket
char buttonSocketRxFrameBuffer[UART_BUFF_LEN];
char buttonSocketTxFrameBuffer[UART_BUFF_LEN];

SerLink::Frame buttonSocketRxFrame(buttonSocketRxFrameBuffer);
SerLink::Frame buttonSocketTxFrame(buttonSocketTxFrameBuffer);

// ButtonEvents for EventQueue for buttonSocket
HardMod::Std::ButtonEvent bt0Ev0, bt0Ev1, bt0Ev2;
HardMod::Std::ButtonEvent* bt0Ev[3] = {&bt0Ev0, &bt0Ev1, &bt0Ev2};

// EventQueue for buttonSocket
HardMod::EventQueue bt0EvQueue(bt0Ev, 3);

// General purpose ButtonEvent for buttonSocket
HardMod::Std::ButtonEvent bt0Event;

SerLink::Socket buttonSocket(&writer0, &reader0, "BUT01", &buttonSocketRxFrame, &buttonSocketTxFrame, &bt0Event, &bt0EvQueue);

//-----------------------

//-----------------------
// general purpose socket data buffers

#define SOCKET_RX_DATA_LEN 12
char socketRxData[SOCKET_RX_DATA_LEN] = {0};
char socketTxData[SOCKET_RX_DATA_LEN] = {0};
uint16_t socketRxDataLen;
//-----------------------

HardMod::Std::ButtonEvent button0Event, button0EventExt;

// Button: Pin B3 (Pin 11)
HardMod::Std::ButtonModule button0(GPIO_REG__PORTB, 3, false, &button0Event, true, 150);

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
  swTimer_tickReset(&buttonStartTick);

  //strncpy(ledSocketPayload, "a1", 2);

  //--------------------------
  // Green Led
  // Pin B5 (Pin 13)
  gpio_setPinDirection(GPIO_REG__PORTB, 5, GPIO_PIN_DIRECTION__OUT);
  //gpio_setPinDirection(GPIO_REG__PORTB, 5, GPIO_PIN_DIRECTION__IN);

  gpio_setPinLow(GPIO_REG__PORTB, 5);
  //--------------------------
  // Red Led
  // Pin B4 (Pin 12)
  gpio_setPinDirection(GPIO_REG__PORTB, 4, GPIO_PIN_DIRECTION__OUT);
  //gpio_setPinDirection(GPIO_REG__PORTB, 5, GPIO_PIN_DIRECTION__IN);

  gpio_setPinLow(GPIO_REG__PORTB, 4);
  //--------------------------
  // Pin B4 (Pin 12)
  //gpio_setPinDirection(GPIO_REG__PORTB, 4, GPIO_PIN_DIRECTION__IN);

  sprintf(txData, "AAA", nullptr);

  //--------------------------
  HardMod::Std::HwModule::Adc_init(HardMod::Std::HwModule::PS_128);

  // Pin A0 (ADC0 - Pin23)
  HardMod::Std::HwModule::Adc_setInput(HardMod::Std::HwModule::ADC0);

  HardMod::Std::HwModule::Adc_startConversion();

} // end setup()


void loop() {

  static bool send = false;
  static bool frameSent = false;
  static uint8_t pressDuration;
  
  uint16_t txFrameCount = 0;
  
  //cli();
  button0.run();
  // HardMod::Std::ButtonModule::eventTypes event = button0.getEvent(&pressDuration);
  // if(event == HardMod::Std::ButtonModule::eventTypes::Pressed)
  // {
  //   sprintf(socketTxData, "%s\0", "PR");
  //   buttonSocket.sendData(socketTxData, 2, true);
  // }
  // else if(event == HardMod::Std::ButtonModule::eventTypes::Released)
  // {
  //   sprintf(socketTxData, "%s\0", "RL");
  //   buttonSocket.sendData(socketTxData, 2, true);
  // }
  if(button0.getEvent(&button0EventExt) == true)
  {
    char action = button0EventExt.getAction();
    if(action == BUTTONEVENT__RELEASED)
    {
      //gpio_setPinHigh(GPIO_REG__PORTB, 5);
    }
    button0Event.clear();

    bool ok = buttonSocket.sendEvent(button0EventExt, socketTxData, true);
    if(!ok){
      gpio_setPinHigh(GPIO_REG__PORTB, 5);
    }
    
    // if(action == BUTTONEVENT__PRESSED)
    // {
    //   // uint8_t len = button0Event.serialise(socketTxData);
    //   // buttonSocket.sendData(socketTxData, len, true);     
    //   buttonSocket.sendEvent(button0Event, socketTxData, true);
    // }
    //button0Event.clear();
  }
  //sei();
  buttonSocket.run();

  /*
  cli();
  if(swTimer_tickCheckTimeout(&buttonStartTick, 20))
  {
    if(buttonListen)
    {
      bool pin = gpio_getPinState(GPIO_REG__PORTB, 4);
      if(pin)
      {
        //gpio_setPinHigh(GPIO_REG__PORTB, 5);
      }
      else
      {
        // first edge
        //gpio_setPinLow(GPIO_REG__PORTB, 5);
        buttonListen = false;
        buttonCount = 0;

        sprintf(socketTxData, "%s\0", "PR");
        buttonSocket.sendData(socketTxData, 2, true);
      }
    }
    else
    {
      buttonCount++;
      if(buttonCount >= 10)
      {
        buttonListen = true;
      }
    }
    
  }
  sei();
  */

  cli();
  if(swTimer_tickCheckTimeout(&startTick, 3000))
  {
    // sprintf(txData, "%03d", count++);
    // buttonSocket.sendData(txData, 3, true);
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

  if(ledSocket.getRxData(socketRxData, &socketRxDataLen)) // LED01T805002G1 LED01T805002G0 (old: LED01T8050011  LED01T8050010)
  {
    if(socketRxData[0] == 'G')
    {
      // Green Led
      if(socketRxData[1] == '1')
      {
        gpio_setPinHigh(GPIO_REG__PORTB, 5);
      }
      else if(socketRxData[1] == '0')
      {
        gpio_setPinLow(GPIO_REG__PORTB, 5);
      }
      else{ /* do nothing */ }
    }
    else if(socketRxData[0] == 'R') // LED01T805002R1 LED01T805002R0
    {
      // Red Led
      if(socketRxData[1] == '1')
      {
        gpio_setPinHigh(GPIO_REG__PORTB, 4);
      }
      else if(socketRxData[1] == '0')
      {
        gpio_setPinLow(GPIO_REG__PORTB, 4);
      }
      else{ /* do nothing */ }
    }
    
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
  //reader0.clearRxFlag();
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