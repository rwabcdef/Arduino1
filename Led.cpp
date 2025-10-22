#include "Led.hpp"
#include "swTimer.h"
#include "hw_gpio.h"
#include "SerLink_Utils.hpp"

namespace HardMod::Std
{

LedEvent::LedEvent(): Event()
{
  this->type = None;
}

LedEvent::eventTypes LedEvent::getType(LedFlashParams* flashParams)
{
  if(flashParams != nullptr && this->type == Flash)
  {
    flashParams->numFlashes = this->numFlashes;
    flashParams->onPeriods = this->onPeriods;
    flashParams->offPeriods = this->offPeriods;
  }
  return this->type;
}

bool LedEvent::deSerialise(char* str)
{
  this->clr(); // base class clr()

  this->setId(str[0]);
  this->setAction(str[1]);

  switch(this->action)
  {
    case LEDEVENT__ON:
      this->type = On;
      break;
    case LEDEVENT__OFF:
      this->type = Off;
      break;
    case LEDEVENT__FLASH:
      this->type = Flash;
      this->numFlashes = SerLink::Utils::strToUint8(&str[2], 2);
      this->onPeriods = SerLink::Utils::strToUint8(&str[4], 2);
      this->offPeriods = SerLink::Utils::strToUint8(&str[6], 2);
      break;
    default:
      this->type = None;
      return false;
  }
  return true;
}

void LedEvent::clear()
{
  this->clr();         // base class clr()
  this->type = None;
  this->numFlashes = 0;
  this->onPeriods = 0;
  this->offPeriods = 0;
}

void LedEvent::copy(Event* copyEvent)
{
  LedEvent* copy = (LedEvent*) copyEvent;
  copy->setAck(this->ack);
  copy->setId(this->getId());
  copy->setAction(this->action);
  copy->type = this->type;
  copy->numFlashes = this->numFlashes;
  copy->onPeriods = this->onPeriods;
  copy->offPeriods = this->offPeriods;
}
//-----------------------------------------------------------------------------------

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