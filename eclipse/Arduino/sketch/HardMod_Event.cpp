
#include "HardMod_Event.hpp"

namespace HardMod
{

Event::Event()
{
  this->action = EVENT__NONE;
}

void Event::setAction(char action)
{
  this->action = action;
}

char Event::getAction()
{
  return this->action;
}

char Event::getActionClear()
{
  char action = this->action;
  this->clr();
  return action;
}

uint8_t Event::serialise(char* str)
{
  uint8_t index = 0;
  str[index++] = this->action;
  return index;
}

bool Event::deSerialise(char* str)
{
  uint8_t index = 0;
  this->action = str[index++];
  return true;
}

void Event::copy(Event* copyEvent)
{
  this->action = copyEvent->action;
}

void Event::clear()
{
  this->clr();
}

void Event::clr()
{
  this->action = EVENT__NONE;
}

//------------------------------------------------------------
HasUnitId::HasUnitId(char unitId): unitId(unitId) {}

void HasUnitId::setUnitId(char unitId)
{
  this->unitId = unitId;
}

} // end namespace HardMod