
#ifndef MOTOR_HPP_
#define MOTOR_HPP_

#include <stdint.h>
#include <stdbool.h>
#include "pwm_common.h"
#include "idChar.hpp"
#include "HardMod_Event.hpp"

#define MOTOREVENT__SET_PERCENT 'P'
#define MOTOREVENT__SET_DIRECTION 'D'
#define MOTOREVENT__SET_FREQUENCY 'F'

// Used by a socket to request all motor parameters
// (values are returned in the ack frame, by the instant handler)
#define MOTOREVENT__GET_ALL 'G'

#define MOTOREVENT__DIRECTION_FORWARD 'F'
#define MOTOREVENT__DIRECTION_REVERSE 'R'
#define MOTOREVENT__DIRECTION_DISABLED 'D'

#define MOTOREVENT__FREQUENCY_500_HZ '0'
#define MOTOREVENT__FREQUENCY_1_KHZ '1'
#define MOTOREVENT__FREQUENCY_2_KHZ '2'
#define MOTOREVENT__FREQUENCY_5_KHZ '3'
#define MOTOREVENT__FREQUENCY_10_KHZ '4'
#define MOTOREVENT__FREQUENCY_20_KHZ '5'

namespace HardMod::Std
{

class MotorEvent: public Event, public VariableIdChar {
  
  public:
    enum eventTypes {
      None = 0,
      SetPercent,
      SetDirection,
      SetFrequency
    };
    // enum directionValues {
    //   Forward = 0,
    //   Reverse,
    //   Disabled
    // };

    MotorEvent();
    eventTypes getType(uint8_t* value = nullptr);

    //--------------------
    // Over-ridden base class methods

    bool deSerialise(char* str) override;

    void clear() override;

    void copy(Event* copyEvent) override;
    //--------------------
  protected:
    eventTypes type;
    uint8_t value; // percent, direction or pwm frequency code
};

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

    static void clr();

    Motor(pwmTypes pwm, uint8_t pinAPort, uint8_t pinAPin, uint8_t pinBPort,
      uint8_t pinBPin, pwmFreqValues frequency = PWM_FREQ_1_KHZ);
    uint8_t init();
    void setPercent(uint8_t percent);
    uint8_t getPercent();
    bool setDirection(directionStates direction);
    directionStates getDirection();
    bool setFrequency(pwmFreqValues frequency);
    pwmFreqValues getFrequency();

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
  
}; // end class Motor

} // end namespace HardMod::Std
#endif