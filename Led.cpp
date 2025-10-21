#include "Led.hpp"
#include "swTimer.h"
#include "hw_gpio.h"

namespace HardMod::Std
{

#define STATE_OFF 0
#define STATE_ON 1
#define STATE_FLASH_ON 2
#define STATE_FLASH_OFF 3

Led::Led(int8_t port, uint8_t pin)
: port(port), pin(pin), numFlashes(0), onPeriods(0), offPeriods(0)
{
  this->type = LedEvent::None;
  this->currentState = STATE_OFF;
  gpio_setPinDirection(this->port, this->pin, GPIO_PIN_DIRECTION__OUT);
  swTimer_tickReset(&this->startTick);
}

void Led::on()
{
  this->type = LedEvent::On;
  //gpio_setPinState(this->port, this->pin, GPIO_PIN_STATE__HIGH);
}

void Led::off()
{
  this->type = LedEvent::Off;
}

void Led::flash(uint8_t numFlashes, uint8_t onPeriods, uint8_t offPeriods)
{
  this->type = LedEvent::Flash;
  this->numFlashes = numFlashes;
  this->onPeriods = onPeriods;
  this->offPeriods = offPeriods;
  //swTimer_tickReset(&this->startTick);
}

void Led::run()
{
  if(swTimer_tickCheckTimeout(&this->startTick, LED_PERIOD_mS))
  {
    this->common();
    switch(this->currentState)
    {
      case STATE_OFF:
        this->currentState = this->offState();
        break;
      case STATE_ON:
        this->currentState = this->onState();
        break;
      case STATE_FLASH_ON:
        this->currentState = this->flashOnState();
        break;
      case STATE_FLASH_OFF:
        this->currentState = this->flashOffState();
        break;
    }
  }
}

void Led::common()
{
  if(this->type == LedEvent::On)
  {
    this->currentState = STATE_ON;
    gpio_setPinHigh(this->port, this->pin);
  }
  else if(this->type == LedEvent::Off)
  {
    this->currentState = STATE_OFF;
    gpio_setPinLow(this->port, this->pin);
  }
  else if(this->type == LedEvent::Flash)
  {
    this->currentState = STATE_FLASH_ON;
    this->flashCount = 0;
    this->periodCount = 0;
    gpio_setPinHigh(this->port, this->pin);
  }
  this->type = LedEvent::None; // clear event type
}

uint8_t Led::onState(){
  return STATE_ON;
}

uint8_t Led::offState(){
  return STATE_OFF;
}

uint8_t Led::flashOnState(){
  if(this->periodCount > this->onPeriods)
  {
    this->periodCount = 0;
    gpio_setPinLow(this->port, this->pin);
    return STATE_FLASH_OFF;
  }
  else
  {
    this->periodCount++;
    return STATE_FLASH_ON;
  }
}

uint8_t Led::flashOffState(){
  if(this->periodCount > this->offPeriods)
  {
    this->periodCount = 0;
    this->flashCount++;
    if(this->flashCount >= this->numFlashes)
    {
      gpio_setPinLow(this->port, this->pin);
      return STATE_OFF;
    }
    else
    {
      gpio_setPinHigh(this->port, this->pin);
      return STATE_FLASH_ON;
    }
  }
  else
  {
    this->periodCount++;
    return STATE_FLASH_OFF;
  }
}

} // namespace HardMod::Std