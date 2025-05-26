#include "ButtonModule.hpp"
#include "swTimer.h"

namespace EventModule::Std
{

#define RELEASED 1
#define PRESSED 1

ButtonModule::ButtonModule(uint8_t port, uint8_t pin, bool pressedPinState)
: port(port), pin(pin), pressedPinState(pressedPinState)
{
  this->state = RELEASED;
  this->eventPinState = this->pressedPinState;
  this->stable = true;
  this->previousPinState = !this->pressedPinState;
}

void ButtonModule::run()
{
  
}

} // end namespace EventModule::Std