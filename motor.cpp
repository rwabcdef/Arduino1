
#include "motor.hpp"
#include "hw_gpio.h"

namespace HardMod::Std
{

  Motor(pwmTypes pwm, uint8_t pinAPort, uint8_t pinAPin, uint8_t pinBPort,
    uint8_t pinBPin, pwmFreqValues frequency = PWM_FREQ_1_KHZ)
    : pwm(pwm), frequency(frequency)
{
  // this->pwm = pwm;
  // this->frequency = frequency;

  this->pinAPort = 0; // invalid
  this->pinAPin = 0;  // invalid
  this->pinBPort = 0; // invalid
  this->pinBPin = 0;  // invalid

  if(pinAPort >= GPIO_REG__PORTB && pinAPort <= GPIO_REG__PORTD)
  {
    this->pinAPort = pinAPort;
    if(pinAPin <= 7)
    {
      this->pinAPin = pinAPin;
    }
  }

  if(pinBPort >= GPIO_REG__PORTB && pinBPort <= GPIO_REG__PORTD)
  {
    this->pinPort = pinBPort;
    if(pinBPin <= 7)
    {
      this->pinBPin = pinBPin;
    }
  }
    
  this->percent = 0;
  this->direction = F;

  // Initialize the selected PWM channel
  if(this->pwm == PWM0)
  {
    pwm0_init();
    pwm0_setFrequency(this->frequency);
    pwm0_setDutyPercent(0);
  }
  else if(this->pwm == PWM1)
  {
    pwm1_init();
    pwm1_setFrequency(this->frequency);
    pwm1_setDutyPercent(0);
  }
}
void Motor::setPercent(uint8_t percent)
{
  if(percent > 100)
    percent = 100;

  this->percent = percent;

  if(this->pwm == PWM0)
  {
    pwm0_setDutyPercent(this->percent);
  }
  else if(this->pwm == PWM1)
  {
    pwm1_setDutyPercent(this->percent);
  }
}
uint8_t Motor::getPercent()
{
  return this->percent;
}

void Motor::setDirection(directionStates direction)
{
  this->direction = direction;  
  // Here you would typically set GPIO pins to control motor direction
}

Motor::directionStates Motor::getDirection()
{
  return this->direction; 
}
} // end namespace HardMod::Std