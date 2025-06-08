#include "ButtonModule.hpp"
#include "swTimer.h"
#include "hw_gpio.h"

namespace HardwareModule::Std
{

#define RELEASED 1
#define PRESSED 1

ButtonModule::ButtonModule(uint8_t port, uint8_t pin, bool pressedPinState)
: port(port), pin(pin), pressedPinState(pressedPinState)
{
  this->currentState = RELEASED;
  this->eventPinState = this->pressedPinState;
  this->stable = true;
  this->previousPinState = !this->pressedPinState;
  this->eventType = None;
}

void ButtonModule::run()
{
  switch(this->currentState)
	{
		case RELEASED: { this->currentState = this->released(); break;}
		case PRESSED: { this->currentState = this->pressed(); break;}
	}
}

ButtonModule::eventTypes ButtonModule::getEvent(uint16_t* pressDuration)
{
  eventTypes ret = this->eventType;
  this->eventType = None;
  return ret;
}
//---------------------------------------------------
ButtonModule::internalEventTypes ButtonModule::eventCheck()
{
  internalEventTypes ret = None;
  bool currentPinState = gpio_getPinState(this->port, this->pin);
  if(this->stable)
  {
    if((currentPinState == this->eventPinState) &&
      (this->previousPinState != this->eventPinState))
      {
        // edge detected
        ret = Edge;
      }
  }
}
//---------------------------------------------------

// state methods
uint8_t ButtonModule::released()
{

}
uint8_t ButtonModule::pressed()
{
  
}
//---------------------------------------------------


} // end namespace EventModule::Std