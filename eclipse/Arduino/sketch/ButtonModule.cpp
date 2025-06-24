#include "ButtonModule.hpp"
#include "swTimer.h"
#include "hw_gpio.h"

namespace HardMod::Std
{

#define RELEASED 1
#define PRESSED 2

#define STABLE_ACTIVE_THRESHOLD 5
#define STABLE_INACTIVE_THRESHOLD 5

#define RUN_PERIOD_mS 20

ButtonModule::ButtonModule(uint8_t port, uint8_t pin, bool pressedPinState)
: port(port), pin(pin), pressedPinState(pressedPinState)
{
  this->currentState = RELEASED;
  this->eventPinState = this->pressedPinState;
  this->stable = true;
  this->previousPinState = !this->pressedPinState;
  this->eventType = None;

  gpio_setPinDirection(this->port, this->pin, GPIO_PIN_DIRECTION__IN);
  swTimer_tickReset(&this->startTick);
}

void ButtonModule::run()
{
  if(swTimer_tickCheckTimeout(&this->startTick, 20))
  {
    switch(this->currentState)
    {
      case RELEASED: { this->currentState = this->released(); break;}
      case PRESSED: { this->currentState = this->pressed(); break;}
    }
  }
}

ButtonModule::eventTypes ButtonModule::getEvent(uint8_t* pressDuration)
{
  eventTypes ret = this->eventType;
  if(this->eventType == Released)
  {
    *pressDuration = this->pressedCount;
  }
  this->eventType = None;  // clear event
  return ret;
}
//---------------------------------------------------
ButtonModule::internalEventTypes ButtonModule::eventCheck()
{
  ButtonModule::internalEventTypes ret = NoEvent;
  bool currentPinState = gpio_getPinState(this->port, this->pin);
  if(this->stable)
  {
    if((currentPinState == this->eventPinState) &&
      (this->previousPinState != this->eventPinState))
      {
        // edge detected
        ret = Edge;
        this->activeCount = 0;
        this->inActiveCount = 0;
        this->stable = false;
      }
  }
  else
  {
    // currently not stable
    if(currentPinState == this->eventPinState)
    {
      this->inActiveCount = 0;
      this->activeCount++;
      if(this->activeCount >= STABLE_ACTIVE_THRESHOLD)
      {
        // button is stable & active
        this->stable = true;
        ret = StableActive;
      }
    }
    else
    {
      this->activeCount = 0;
      this->inActiveCount++;
      if(this->inActiveCount >= STABLE_INACTIVE_THRESHOLD)
      {
        // button is stable & inactive
        this->stable = true;
        ret = StableInactive;
      }
    }
  }
  this->previousPinState = currentPinState;
  return ret;
}
//---------------------------------------------------

// state methods
uint8_t ButtonModule::released()
{
  ButtonModule::internalEventTypes event = this->eventCheck();
  if(event == Edge)
  {
    this->eventType = Pressed;
    this->pressedCount = 0;
    return PRESSED;
  }
  return RELEASED;
}

uint8_t ButtonModule::pressed()
{
  ButtonModule::internalEventTypes event = this->eventCheck();
  if(event == NoEvent)
  {
    this->pressedCount++;
    return PRESSED;
  }
  else if(event == StableActive)
  {
    // change the polarity of the edge event state
    this->eventPinState = (!this->eventPinState);
    return PRESSED;
  }
  else if(event == StableInactive)
  {
    // button is inactive - so return to released state.
    // This situation will occur with a very quick button press.
    this->eventPinState = this->pressedPinState;
    return RELEASED;
  }
  else if(event == Edge)
  {
    this->eventType = Released;
    this->eventPinState = this->pressedPinState;
    return RELEASED;
  }
}
//------------------- --------------------------------


} // end namespace EventModule::Std
