

#ifndef BUTTONMODULE_HPP_
#define BUTTONMODULE_HPP_

#include <stdint.h>
#include <stdbool.h>
#include "StateMachine.hpp"

#define BUTTONMODULE_THRESHOLD 4
#define BUTTONMODULE_PERIOD_mS 50

namespace ArdMod::Std
{

class ButtonModule
{
  private:    
    uint8_t port;
    uint8_t pin;
    uint8_t count;
    uint16_t duration;

  public:
    ButtonModule(uint8_t port, uint8_t pin);

    //---------------------------------------------------
    // Basic interface

    // Returns true if button pressed otherwise returns false.
    bool getPress();

    // Returns length of ButtonModule periods if button has been released,
    // otherwise returns 0.
    uint16_t getRelease();
    //---------------------------------------------------
};

} // end namespace ArdMod::Std

#endif