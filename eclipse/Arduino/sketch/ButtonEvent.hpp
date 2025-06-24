#ifndef BUTTONEVENT_HPP_
#define BUTTONEVENT_HPP_

#include "HardMod_Event.hpp"
#include "ButtonModule.hpp"

#define BUTTONEVENT__PRESSED 'P'
#define BUTTONEVENT__LONGPRESS 'L'
#define BUTTONEVENT__RELEASED 'R'
#define BUTTONEVENT__STUCK 'S'

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