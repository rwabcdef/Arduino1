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
    uint8_t serialise(char* str) override;

    void clear() override;

    void copy(Event* copyEvent) override;
    //--------------------
};

//------------------------------------------------------------
// ButtonConfigEvent

#define BUTTONCONFIGEVENT__LONGPRESS 'L'
#define BUTTONCONFIGEVENT__RELEASE 'R'

class ButtonConfigEvent: public Event, public VariableIdChar
{
  protected:
    uint8_t value;
    // uint8_t longPressThreshold;
    // bool enableRelease;

  public:
    ButtonConfigEvent();
    
    uint8_t getLongPressThreshold();

    bool getEnableRelease();

    //--------------------
    // Over-ridden base class methods
    bool deSerialise(char* str);

    void clear();

    void copy(Event* copyEvent);
    //--------------------

};


} // end namespace HardMod

#endif