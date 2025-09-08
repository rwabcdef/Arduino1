

#ifndef BUTTONMODULE_HPP_
#define BUTTONMODULE_HPP_

#include <stdint.h>
#include <stdbool.h>
#include "StateMachine.hpp"
#include "ButtonEvent.hpp"

#define BUTTONMODULE_THRESHOLD 4
#define BUTTONMODULE_PERIOD_mS 50

namespace HardMod::Std
{

class ButtonModule : public StateMachine
{
  public:
    enum eventTypes{
      None = 0,
      Pressed,
      LongPressed,
      Released,
      Stuck
    };

    ButtonModule(uint8_t port, uint8_t pin, bool pressedPinState,
      ButtonEvent *buttonEvent = nullptr, bool releaseActive = false, uint8_t longPressThreshold = 0);
    
    void run();

    //---------------------------------------------------
    // Basic (flag) interface

    // Returns true if button pressed otherwise returns false.
    bool getPress();

    // Returns length of ButtonModule periods if button has been released,
    // otherwise returns 0.
    uint16_t getRelease();

    eventTypes getEvent(uint8_t* pressDuration);
    //---------------------------------------------------
    // Event object interface

    bool getEvent(ButtonEvent* event = nullptr);
    //---------------------------------------------------

  protected:    
    uint8_t port;
    uint8_t pin;
    ButtonEvent *buttonEvent;
    uint8_t longPressThreshold;
    bool releaseActive;
    bool pressedPinState;
    bool eventPinState;
    bool currentPinState;
    bool previousPinState;
    bool outEventFlag;
    bool stable;
    uint8_t activeCount;
    uint8_t inActiveCount;
    uint8_t pressedCount;
    //uint16_t duration;
    uint16_t startTick;
    eventTypes eventType;

    // state methods
    uint8_t released();
    uint8_t pressed();    

    enum internalEventTypes{
      NoEvent = 0,
      Edge = 1,
      StableActive,
      StableInactive,
      StableActiveLong
    };

    internalEventTypes eventCheck();
};

} // end namespace HardMod::Std

#endif