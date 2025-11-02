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
#include "Button.hpp"
#include "HwModule.hpp"
#include "HardMod_EventQueue.hpp"
#include "Adc.hpp"
#include "pot.hpp"
#include "Led.hpp"
#include "Registers.hpp"
#include "pwm_common.h"

/*
Hardware Config

Green Led: Pin 13 (PB5)
Red Led: Pin 12 (PB4)
Button0: Pin 11 (PB3)
*/

void toggleBuiltInLed();
bool debugSockInstantHandler(SerLink::Frame &rxFrame, uint16_t* dataLen, char* data);
uint8_t calcADCPercentage(uint16_t value);
// void pwm_init_pin10();
// void pin10PwmPercent(uint8_t percent);

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

HardMod::Std::LedEvent ledEvent;

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
// ADC
HardMod::Std::AdcInput potAadcInput(HardMod::Std::HwModule::AdcInputValues::ADC0);

HardMod::Std::AdcInput* AdcInputs[] = {&potAadcInput};

HardMod::Std::Adc adc(AdcInputs, 1);
//-----------------------
// PotA

HardMod::Std::PotEvent potEvent; // event for all Pots

HardMod::Std::Pot potA('A', &adc, 0);
//-----------------------
// pot0 socket
char pot0SocketRxFrameBuffer[UART_BUFF_LEN];
char pot0SocketTxFrameBuffer[UART_BUFF_LEN];

SerLink::Frame pot0SocketRxFrame(echoSocketRxFrameBuffer);
SerLink::Frame pot0SocketTxFrame(echoSocketTxFrameBuffer);

// PotEvents for EventQueue for potSocket
HardMod::Std::PotEvent ptEv0, ptEv1, ptEv2;
HardMod::Std::PotEvent* ptEv[3] = {&ptEv0, &ptEv1, &ptEv2};

// EventQueue for potSocket
HardMod::EventQueue ptEvQueue(ptEv, 3);

// General purpose ButtonEvent for buttonSocket
HardMod::Std::PotEvent ptEvent;

//SerLink::Socket buttonSocket(&writer0, &reader0, "BUT01", &buttonSocketRxFrame, &buttonSocketTxFrame, &bt0Event, &bt0EvQueue);

SerLink::Socket potSocket(&writer0, &reader0, "POT01", &pot0SocketRxFrame, &pot0SocketTxFrame, &ptEvent, &ptEvQueue);
//-----------------------
// general purpose socket data buffers

#define SOCKET_RX_DATA_LEN 12
char socketRxData[SOCKET_RX_DATA_LEN] = {0};
char socketTxData[SOCKET_RX_DATA_LEN] = {0};
uint16_t socketRxDataLen;
//-----------------------

HardMod::Std::ButtonEvent buttonAEvent; //, button0EventExt;

// Button: Pin B3 (Pin 11)
HardMod::Std::Button buttonA('A', GPIO_REG__PORTB, 3, false, true, 150);

//-----------------------

HardMod::Std::Led greenLed(GPIO_REG__PORTB, 5);
HardMod::Std::Led redLed(GPIO_REG__PORTB, 4);
//-----------------------


//SerLink::Frame txFrame("TST16", SerLink::Frame::TYPE_TRANSMISSION, 452, 4, "abcd");
//static SerLink::Frame txFrame("TST16", SerLink::Frame::TYPE_TRANSMISSION, 452, txFrameBuffer, 4, "abcd");

uint8_t count = 0;
static char txData[4];

// void pwm_init_pin10()
// {
//     DDRB |= (1 << PB2);          // Set PB2 (pin 10) as output

//     // --- Configure Timer1 for 8-bit Fast PWM mode ---
//     // WGM10 = 1, WGM12 = 1 → Fast PWM 8-bit
//     TCCR1A = (1 << WGM10) | (1 << COM1B1);
//     TCCR1B = (1 << WGM12) | (1 << CS11);   // Prescaler = 8

//     OCR1B = 51; // 20% duty (51 / 255)
// }

// void pin10PwmPercent(uint8_t percent)
// {
//   if(percent > 100)
//   {
//     percent = 100;
//   }
//   uint8_t value = percent << 1;
//   value += percent >> 1;
//   OCR1B = value;
// }

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(7, OUTPUT);

  //uart_init(rxBuffer, UART_BUFF_LEN);

  reader0.init();

  timer0_init();

  //pwm_init_pin10();
  pwm0_init();
  pwm0_setFrequency(PWM_FREQ_1_KHZ);

  swTimer_tickReset(&startTick);
  swTimer_tickReset(&buttonStartTick);

  //strncpy(ledSocketPayload, "a1", 2);

  //--------------------------
  // Green Led
  // Pin B5 (Pin 13)
  //gpio_setPinDirection(GPIO_REG__PORTB, 5, GPIO_PIN_DIRECTION__OUT);
  //gpio_setPinDirection(GPIO_REG__PORTB, 5, GPIO_PIN_DIRECTION__IN);

  //gpio_setPinLow(GPIO_REG__PORTB, 5);
  //--------------------------
  // Red Led
  // Pin B4 (Pin 12)
  //gpio_setPinDirection(GPIO_REG__PORTB, 4, GPIO_PIN_DIRECTION__OUT);
  //gpio_setPinDirection(GPIO_REG__PORTB, 5, GPIO_PIN_DIRECTION__IN);

  //gpio_setPinLow(GPIO_REG__PORTB, 4);
  //--------------------------
  // Pin B4 (Pin 12)
  //gpio_setPinDirection(GPIO_REG__PORTB, 4, GPIO_PIN_DIRECTION__IN);

  sprintf(txData, "AAA", nullptr);

  //gpio_setPinDirection(GPIO_REG__PORTB, 2, GPIO_PIN_DIRECTION__OUT);  // Pin 10 / PB2
  //--------------------------
  // HardMod::Std::HwModule::Adc_init(HardMod::Std::HwModule::PS_128);

  // // Pin A0 (ADC0 - Pin23)
  // HardMod::Std::HwModule::Adc_setInput(HardMod::Std::HwModule::ADC0);

  // HardMod::Std::HwModule::Adc_startConversion();

  //gpio_setPinHigh(GPIO_REG__PORTB, 5);

} // end setup()


void loop() {

  static bool send = false;
  static bool frameSent = false;
  static uint8_t pressDuration;
  
  uint16_t txFrameCount = 0;
  //static uint8_t prevAdcPercent;
  static uint8_t previous = 150;
  static bool testPinOn = false;  // pin PB2
  
  //cli();
  buttonA.run();
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
  if(buttonA.getEvent(&buttonAEvent) == true)
  {
    char action = buttonAEvent.getAction();
    if(action == BUTTONEVENT__RELEASED)
    {
      //gpio_setPinHigh(GPIO_REG__PORTB, 5);
    }
    //buttonAEvent.clear();

    bool ok = buttonSocket.sendEvent(buttonAEvent, socketTxData, true);
    if(!ok){
      //gpio_setPinHigh(GPIO_REG__PORTB, 5);
    }
    
    // if(action == BUTTONEVENT__PRESSED)
    // {
    //   // uint8_t len = button0Event.serialise(socketTxData);
    //   // buttonSocket.sendData(socketTxData, len, true);     
    //   buttonSocket.sendEvent(button0Event, socketTxData, true);
    // }
    buttonAEvent.clear();
  }
  //sei();
  // buttonSocket.run();
  // echoSocket.run();

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

  if(potA.getEvent(&potEvent))
  {
    // uint8_t len = potEvent.serialise(txData);
    // potSocket.sendData(txData, len, false);

    potSocket.sendEvent(potEvent, socketTxData, false);

    //pin10PwmPercent(potEvent.getPercent());
    pwm0_setDutyPercent(potEvent.getPercent());
  }

  // cli();
  // if(swTimer_tickCheckTimeout(&startTick, 250))
  // {
  //   uint8_t current;
  //   if(adc.getValue(0, &current))
  //   {
  //     if(current != previous){
  //       sprintf(txData, "%03d", current);
  //       pot0Socket.sendData(txData, 3, false);
  //     }
  //     previous = current;
  //   }
  // }
  // sei();

  // cli();
  // if(swTimer_tickCheckTimeout(&startTick, 20))
  // {
  //   if(testPinOn)
  //   {
  //     testPinOn = false;
  //     gpio_setPinLow(GPIO_REG__PORTB, 2); // PB2 = pin 10
  //   }
  //   else
  //   {
  //     testPinOn = true;
  //     gpio_setPinHigh(GPIO_REG__PORTB, 2);
  //   }
  // }
  // sei();

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

  // if(ledSocket.getRxData(socketRxData, &socketRxDataLen)) // LED01T805002G1 LED01T805002G0 (old: LED01T8050011  LED01T8050010)
  // {
  //   if(socketRxData[0] == 'G')
  //   {
  //     // Green Led
  //     if(socketRxData[1] == '1')
  //     {
  //       //gpio_setPinHigh(GPIO_REG__PORTB, 5);
  //       greenLed.on();
  //     }
  //     else if(socketRxData[1] == '0')
  //     {
  //       //gpio_setPinLow(GPIO_REG__PORTB, 5);
  //       greenLed.off();
  //     }
  //     else{ /* do nothing */ }
  //   }
  //   else if(socketRxData[0] == 'R') // LED01T805002R1 LED01T805002R0
  //   {
  //     // Red Led
  //     if(socketRxData[1] == '1')
  //     {
  //       //gpio_setPinHigh(GPIO_REG__PORTB, 4);
  //       redLed.on();
  //     }
  //     else if(socketRxData[1] == '0')
  //     {
  //       //gpio_setPinLow(GPIO_REG__PORTB, 4);
  //       redLed.off();
  //     }
  //     else{ /* do nothing */ }
  //   }
    
  //   memset(socketRxData, 0, SOCKET_RX_DATA_LEN);
  // }

  // LED01T805002G1 LED01T805002G0
  // LED01T805008GF030104 - 3 flashes
  // LED01T805008GF000104 - continuous flashing
  // 
  if(ledSocket.getRxEvent(ledEvent))
  {
    HardMod::Std::LedFlashParams ledFlashParams;
    HardMod::Std::LedEvent::eventTypes type = ledEvent.getType(&ledFlashParams);
    
    if(ledEvent.getId() == 'G')
    {
      if(type == HardMod::Std::LedEvent::eventTypes::On)
      {
        greenLed.on();
      }
      else if(type == HardMod::Std::LedEvent::eventTypes::Off)
      {
        greenLed.off();
      }
      else if(type == HardMod::Std::LedEvent::eventTypes::Flash)
      {
        greenLed.flash(ledFlashParams.numFlashes, ledFlashParams.onPeriods, ledFlashParams.offPeriods);
        //greenLed.flash(3, 1, 4);
      }
    }
    else if(ledEvent.getId() == 'R')
    {
      if(type == HardMod::Std::LedEvent::eventTypes::On)
      {
        redLed.on();
      }
      else if(type == HardMod::Std::LedEvent::eventTypes::Off)
      {
        redLed.off();
      }
      else if(type == HardMod::Std::LedEvent::eventTypes::Flash)
      {
        redLed.flash(ledFlashParams.numFlashes, ledFlashParams.onPeriods, ledFlashParams.offPeriods);
      }
    }
  }

  greenLed.run();
  redLed.run();


  if(echoSocket.getRxData(socketRxData, &socketRxDataLen)) // ECHO1T076004abcd LED01T805003abc DBG01T156003ASD
  {
    
    sprintf(socketTxData, "echo %s\0", socketRxData);
    echoSocket.sendData(socketTxData, socketRxDataLen + 5, false);
    memset(socketRxData, 0, SOCKET_RX_DATA_LEN);

    //gpio_setPinHigh(GPIO_REG__PORTB, 5);
  }

  // MUST check socket for rx data in order to clear flags - even if we are going to ignore the data
  if(debugSocket.getRxData(socketRxData, &socketRxDataLen)) // ECHO1T076004abcd LED01T805003abc DBG01T156003RPB // DBG01T156003ASD DBG01T156003aSD
  {
    // sprintf(socketTxData, "dbg %s\0", socketRxData);
    // echoSocket.sendData(socketTxData, socketRxDataLen + 4, false);
    // memset(socketRxData, 0, SOCKET_RX_DATA_LEN);
  }
  //reader0.clearRxFlag();
  writer0.run();
  reader0.run();

  buttonSocket.run();
  echoSocket.run();
  potSocket.run();

  adc.run();
  potA.run();
  

} // end loop()

bool debugSockInstantHandler(SerLink::Frame &rxFrame, uint16_t* dataLen, char* data) // DBG01T156003RPB
{
  // if(rxFrame.buffer[0] == 'A')
  // {
  //   memset(data, 0, 10); // clear outgoing buffer
  //   strcpy(data, "ABCD\0");
  //   *dataLen = 4;
  //   return true;
  // }
  uint8_t index = 0;
  if(rxFrame.buffer[index++] == 'R')
  {
    if(rxFrame.buffer[index++] == 'P')
    {
      if(rxFrame.buffer[index++] == 'B')
      {
        // read port B
        memset(data, 0, 10); // clear outgoing buffer
        *dataLen = Registers::PortB::Read(data);
        return true;
      }
    }
  }
  else  // readPortB
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