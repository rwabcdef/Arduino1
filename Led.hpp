#ifndef LED_HPP
#define LED_HPP

#include <stdint.h>
#include <stdbool.h>
#include "StateMachine.hpp"
#include "idChar.hpp"
#include "HardMod_Event.hpp"

#define LED_PERIOD_mS 250

namespace HardMod::Std
{

class LedFlashParams{
  public:
    uint8_t numFlashes;
    uint8_t onPeriods;
    uint8_t offPeriods;
};

class LedEvent: public Event, public VariableIdChar {
  
  public:
    enum eventTypes {
      None = 0,
      On,
      Off,
      Flash
    };

    LedEvent();
    eventTypes getType(LedFlashParams* flashParams = nullptr);

    //--------------------
    // Over-ridden base class methods

    bool deSerialise(char* str);

    void clear();

    void copy(Event* copyEvent);
    //--------------------
  protected:
    eventTypes type;
    uint8_t numFlashes;
    uint8_t onPeriods;
    uint8_t offPeriods;
};

class Led : public StateMachine {
  
  public:
    Led(int8_t port, uint8_t pin);
    void on();
    void off();
    void flash(uint8_t numFlashes, uint8_t onPeriods, uint8_t offPeriods);
    void run();

  protected:
    uint8_t port;
    uint8_t pin;
    LedEvent::eventTypes type;
    uint8_t numFlashes;
    uint8_t flashCount;
    uint8_t periodCount;
    uint8_t onPeriods;
    uint8_t offPeriods;
    uint16_t startTick;

    uint8_t onState();
    uint8_t offState();
    uint8_t flashOnState();
    uint8_t flashOffState();
    void common();
};

} // namespace HardMod::Std


#endif