#include "ButtonEvent.hpp"
#include "SerLink_Utils.hpp"

namespace HardMod
{

ButtonEvent::ButtonEvent(): Event()
{
  this->pressDuration = 0;
}

void ButtonEvent::setPressDuration(uint8_t pressDuration)
{
  this->pressDuration = pressDuration;
}

uint8_t ButtonEvent::serialise(char* str)
{
  uint8_t index = 0;
  str[index++] = this->action;

  SerLink::Utils::uint16ToStr((uint16_t) this->pressDuration, &str[index], 3);
  index += 3;

  return index;
}

} // end namespace HardMod