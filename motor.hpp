
#ifndef MOTOR_HPP_
#define MOTOR_HPP_

#include <stdint.h>
#include <stdbool.h>
#include "pwm_common.h"

namespace HardMod::Std
{

class Motor
{
  public:
    enum pwmTypes{
      PWM0 = 0,
      PWM1
    };
    enum directionStates{
      Forward = 0,
      Reverse,
      Disabled
    };

    Motor(pwmTypes pwm, uint8_t pinAPort, uint8_t pinAPin, uint8_t pinBPort,
      uint8_t pinBPin, pwmFreqValues frequency = PWM_FREQ_1_KHZ);
    void setPercent(uint8_t percent);
    uint8_t getPercent();
    void setDirection(directionStates direction);
    directionStates getDirection();

  protected:
    
    pwmTypes pwm;
    uint8_t pinAPort;
    uint8_t pinAPin;
    uint8_t pinBPort;
    uint8_t pinBPin;
    pwmFreqValues frequency;
    uint8_t percent;
    directionStates direction;

    void forward();
    void reverse();
    void disable();
  
}; // end class Motor

} // end namespace HardMod::Std
#endif