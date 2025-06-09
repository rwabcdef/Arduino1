

#ifndef BUTTONMODULE_HPP_
#define BUTTONMODULE_HPP_

#include <stdint.h>
#include <stdbool.h>
#include "StateMachine.hpp"

#define BUTTONMODULE_THRESHOLD 4
#define BUTTONMODULE_PERIOD_mS 50

namespace HardwareModule::Std
{

class ButtonModule : public StateMachine
{
  public:
    enum eventTypes{
      None = 0,
      Pressed,
      Released,
      Stuck
    };

    ButtonModule(uint8_t port, uint8_t pin, bool pressedPinState);
    
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

  private:    
    uint8_t port;
    uint8_t pin;
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
      StableInactive
    };

    internalEventTypes eventCheck();
};

} // end namespace ArdMod::Std

#endif