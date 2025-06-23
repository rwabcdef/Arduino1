#ifndef BUTTONMEVENT_HPP_
#define BUTTONMEVENT_HPP_

#include "HardMod_Event.hpp"

namespace HardMod
{
class ButtonEvent: public Event
{
  protected:
    uint8_t pressDuration;

  public:
    ButtonEvent();
    void setPressDuration(uint8_t pressDuration);

    // Over-ridden base class methods
    uint8_t serialise(char* str);
};

} // end namespace HardMod

#endif