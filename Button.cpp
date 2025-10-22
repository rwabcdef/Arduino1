#include "Button.hpp"
#include "swTimer.h"
#include "hw_gpio.h"

namespace HardMod::Std
{

#define RELEASED 1
#define PRESSED 2

#define STABLE_ACTIVE_THRESHOLD 5
#define STABLE_INACTIVE_THRESHOLD 5

#define RUN_PERIOD_mS 20

Button::Button(char id, uint8_t port, uint8_t pin, bool ,
  bool releaseActive, uint8_t longPressThreshold)
: FixedIdChar(id), port(port), pin(pin), pressedPinState(pressedPinState)
, longPressThreshold(longPressThreshold), releaseActive(releaseActive)
{
  this->currentState = RELEASED;
  this->eventPinState = this->pressedPinState;
  this->stable = true;
  this->previousPinState = !this->pressedPinState;
  this->eventType = None;

  gpio_setPinDirection(this->port, this->pin, GPIO_PIN_DIRECTION__IN);
  swTimer_tickReset(&this->startTick);
}

void Button::enableRelease(bool value){ this->releaseActive = value; }

void Button::setLongPressThreshold(uint8_t value){ this->longPressThreshold = value; }

void Button::run()
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

Button::eventTypes Button::getEvent(uint8_t* pressDuration)
{
  eventTypes ret = this->eventType;
  if(this->eventType == Released)
  {
    *pressDuration = this->pressedCount;
  }
  this->eventType = None;  // clear event (flag inetrface)
  return ret;
}

bool Button::getEvent(ButtonEvent* event)
{
  bool ret = false;
  if(this->eventType == None)
  {
    return false;
  }
  else if(this->eventType == Released)
  {
    event->setAction(BUTTONEVENT__RELEASED);
    event->setId(this->getId());
    event->setPressDuration(this->pressedCount);
    ret = true; 
  }
  else if(this->eventType == Pressed)
  {
    event->setAction(BUTTONEVENT__PRESSED);
    event->setId(this->getId());
    ret = true; 
  }
  else if(this->eventType == LongPressed)
  {
    event->setAction(BUTTONEVENT__LONGPRESS);
    event->setId(this->getId());
    ret = true; 
  }
  else{
    // do nothing
  }
  this->eventType = None;
  return ret;
}
//---------------------------------------------------
Button::internalEventTypes Button::eventCheck()
{
  Button::internalEventTypes ret = NoEvent;
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

    // long press check
    else if((currentPinState != this->eventPinState) && 
    (this->eventPinState != this->pressedPinState))
    {
      this->inActiveCount = 0;
      this->activeCount++;
      if((this->longPressThreshold > 0) && (this->activeCount >= this->longPressThreshold))
      {
        // long press event
        this->activeCount = 0;
        ret = StableActiveLong;
      }
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
uint8_t Button::released()
{
  Button::internalEventTypes event = this->eventCheck();
  if(event == Edge)
  {
    this->eventType = Pressed;    // set event code (flag interface)
    this->pressedCount = 0;

    return PRESSED;
  }
  return RELEASED;
}

uint8_t Button::pressed()
{
  Button::internalEventTypes event = this->eventCheck();
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
    // Button released
    if(this->releaseActive == true)
    {
      this->eventType = Released;    // set event code (flag interface)
    }
    
    this->eventPinState = this->pressedPinState;
    return RELEASED;
  }
  else if(event == StableActiveLong)
  {
    // long press
    this->eventType = LongPressed;    // set event code (flag interface)
    
    return PRESSED;
  }
  return PRESSED;
}
//------------------- --------------------------------


} // end namespace EventModule::Std