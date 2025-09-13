#ifndef BUTTONEVENT_HPP_
#define BUTTONEVENT_HPP_

#include "HardMod_Event.hpp"

#define BUTTONEVENT__PRESSED 'P'
#define BUTTONEVENT__LONGPRESS 'L'
#define BUTTONEVENT__RELEASED 'R'
#define BUTTONEVENT__STUCK 'S'

namespace HardMod::Std
{
class ButtonEvent: public Event
{
  protected:
    uint8_t pressDuration;

  public:
    ButtonEvent();
    void setPressDuration(uint8_t pressDuration);
    uint8_t getPressDuration();

    //--------------------
    // Over-ridden base class methods
    uint8_t serialise(char* str);

    void clear();
    //--------------------
};

} // end namespace HardMod

#endif