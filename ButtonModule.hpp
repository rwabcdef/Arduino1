

#ifndef BUTTONMODULE_HPP_
#define BUTTONMODULE_HPP_

#include <stdint.h>
#include <stdbool.h>
#include "StateMachine.hpp"

#define BUTTONMODULE_THRESHOLD 4
#define BUTTONMODULE_PERIOD_mS 50

namespace EventModule::Std
{

class ButtonModule : public StateMachine
{
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
    //uint16_t duration;
    uint16_t startTick;

    // state methods
    uint8_t released();
    uint8_t pressed();

  public:
    ButtonModule(uint8_t port, uint8_t pin, bool pressedPinState);

    //---------------------------------------------------
    // Basic (flag) interface

    // Returns true if button pressed otherwise returns false.
    bool getPress();

    // Returns length of ButtonModule periods if button has been released,
    // otherwise returns 0.
    uint16_t getRelease();
    //---------------------------------------------------
};

} // end namespace ArdMod::Std

#endif