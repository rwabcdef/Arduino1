
#include "motor.hpp"
#include "hw_gpio.h"

namespace HardMod::Std
{

Motor::Motor(pwmTypes pwm, uint8_t pinAPort, uint8_t pinAPin, uint8_t pinBPort,
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
      gpio_setPinDirection(this->pinAPort, this->pinAPin, GPIO_PIN_DIRECTION__OUT);
    }
  }

  if(pinBPort >= GPIO_REG__PORTB && pinBPort <= GPIO_REG__PORTD)
  {
    this->pinBPort = pinBPort;
    if(pinBPin <= 7)
    {
      this->pinBPin = pinBPin;
      gpio_setPinDirection(this->pinBPort, this->pinBPin, GPIO_PIN_DIRECTION__OUT);
    }
  }
    
  // Initialize the selected PWM channel
  if(this->pwm == Motor::pwmTypes::PWM0)
  {
    pwm0_init();
    pwm0_setFrequency(this->frequency);
    pwm0_setDutyPercent(0);
  }
  else if(this->pwm == Motor::pwmTypes::PWM1)
  {
    pwm1_init();
    pwm1_setFrequency(this->frequency);
    pwm1_setDutyPercent(0);
  }

  this->percent = 0;
  this->direction = Motor::directionStates::Forward;
  this->forward();
}
void Motor::setPercent(uint8_t percent)
{
  if(percent > 100)
    percent = 100;

  this->percent = percent;

  if(this->pwm == Motor::pwmTypes::PWM0)
  {
    pwm0_setDutyPercent(this->percent);
  }
  else if(this->pwm == Motor::pwmTypes::PWM1)
  {
    pwm1_setDutyPercent(this->percent);
  }
}
uint8_t Motor::getPercent()
{
  return this->percent;
}

bool Motor::setDirection(directionStates direction)
{
  if(this->percent != 0)
  {
    // Can only change direction when motor is stopped
    return false;
  }

  Motor::directionStates oldDirection = this->direction;

  switch(direction)
  {
    case Motor::directionStates::Forward:
      if(oldDirection == Motor::directionStates::Forward)
      {
        // No change - already forward
        return true;
      }
      else if(oldDirection == Motor::directionStates::Reverse)
      {
        this->forward();
        this->direction = direction; 
        return true;
      }
      else if(oldDirection == Motor::directionStates::Disabled)
      {
        if(this->pwm == Motor::pwmTypes::PWM0)
        {
          pwm0_enable();
        }
        else if(this->pwm == Motor::pwmTypes::PWM1)
        {
          pwm1_enable();
        }
        this->forward();
        this->direction = direction; 
        return true;
      }
    case Motor::directionStates::Reverse:
      if(oldDirection == Motor::directionStates::Reverse)
      {
        // No change - already reverse
        return true;
      }
      else if(oldDirection == Motor::directionStates::Forward)
      {
        this->reverse();
        this->direction = direction; 
        return true;
      }
      else if(oldDirection == Motor::directionStates::Disabled)
      {
        if(this->pwm == Motor::pwmTypes::PWM0)
        {
          pwm0_enable();
        }
        else if(this->pwm == Motor::pwmTypes::PWM1)
        {
          pwm1_enable();
        }
        this->reverse();
        this->direction = direction; 
        return true;
      }
    case Motor::directionStates::Disabled:
    if(oldDirection == Motor::directionStates::Forward || 
       oldDirection == Motor::directionStates::Reverse)
    {
      if(this->pwm == Motor::pwmTypes::PWM0)
      {
        pwm0_disable();
      }
      else if(this->pwm == Motor::pwmTypes::PWM1)
      {
        pwm1_disable();
      }
      this->direction = direction;
      return true;
    }
    else if(oldDirection == Motor::directionStates::Disabled)
    {
      // No change - already disabled
      return true;
    }
    default:
      return false;
  }
}

Motor::directionStates Motor::getDirection()
{
  return this->direction; 
}

void Motor::forward()
{
  gpio_setPinHigh(this->pinAPort, this->pinAPin);
  gpio_setPinLow(this->pinBPort, this->pinBPin);
}

void Motor::reverse()
{
  gpio_setPinLow(this->pinAPort, this->pinAPin);
  gpio_setPinHigh(this->pinBPort, this->pinBPin);
}

} // end namespace HardMod::Std
