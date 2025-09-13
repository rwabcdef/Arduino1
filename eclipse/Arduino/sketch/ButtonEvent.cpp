#include "ButtonEvent.hpp"
#include "SerLink_Utils.hpp"

namespace HardMod::Std
{

ButtonEvent::ButtonEvent(): Event()
{
  this->pressDuration = 0;
}

void ButtonEvent::setPressDuration(uint8_t pressDuration)
{
  this->pressDuration = pressDuration;
}

uint8_t ButtonEvent::getPressDuration()
{
  return this->pressDuration;
}

uint8_t ButtonEvent::serialise(char* str)
{
  uint8_t index = 0;
  str[index++] = this->action;

  if(this->action == BUTTONEVENT__RELEASED)
  {
    SerLink::Utils::uint16ToStr((uint16_t) this->pressDuration, &str[index], 3);
    index += 3;
  }

  return index;
}

void ButtonEvent::clear()
{
  this->clr();         // base class clr()
  this->pressDuration = 0;
}

} // end namespace HardMod