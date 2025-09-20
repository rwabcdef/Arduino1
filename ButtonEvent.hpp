#ifndef BUTTONEVENT_HPP_
#define BUTTONEVENT_HPP_

#include "HardMod_Event.hpp"
#include "idChar.hpp"

#define BUTTONEVENT__PRESSED 'P'
#define BUTTONEVENT__LONGPRESS 'L'
#define BUTTONEVENT__RELEASED 'R'
#define BUTTONEVENT__STUCK 'S'

namespace HardMod::Std
{
class ButtonEvent: public Event, public VariableIdChar
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

    void copy(Event* copyEvent);
    //--------------------
};

#define BUTTONCONFIGEVENT__LONGPRESS 'L'
#define BUTTONCONFIGEVENT__RELEASE 'R'

class ButtonConfigEvent: public Event
{
  protected:

};


} // end namespace HardMod

#endif