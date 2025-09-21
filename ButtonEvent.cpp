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
  
  str[index++] = this->getId();
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

void ButtonEvent::copy(Event* copyEvent)
{
  ButtonEvent* copy = (ButtonEvent*) copyEvent;
  copy->setAck(this->ack);
  copy->setId(this->getId());
  copy->setAction(this->action);
  copy->setPressDuration(this->pressDuration);
}


ButtonConfigEvent::ButtonConfigEvent()
{
  this->value = 0;
}

bool ButtonConfigEvent::deSerialise(char* str)
{
  uint8_t index = 0;

  this->setId(str[index++]);
  this->action = str[index++];

  if(this->action == BUTTONCONFIGEVENT__LONGPRESS)
  {
    this->value = SerLink::Utils::strToUint8(str[index], 3);
  }
  else if(this->action == BUTTONCONFIGEVENT__RELEASE)
  {
    this->value = str[index] == '0' ? 0 : 1;
  }
  else
  {
    this->value = 0;
  }
  return true;
}

uint8_t ButtonConfigEvent::getLongPressThreshold()
{
  return this->value;
}

bool ButtonConfigEvent::getEnableRelease()
{
  return this->value == 0 ? false : true;
}

} // end namespace HardMod