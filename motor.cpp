
#include "motor.hpp"
#include "hw_gpio.h"
#include "SerLink_Utils.hpp"

namespace HardMod::Std
{

MotorEvent::MotorEvent(): Event()
{
  this->type = None;
}

MotorEvent::eventTypes MotorEvent::getType(uint8_t* value)
{
  if(value != nullptr && (this->type == SetPercent || this->type == SetDirection))
  {
    *value = this->value;
  }
  return this->type;
}

bool MotorEvent::deSerialise(char* str)
{
  this->clr(); // base class clr()

  this->setId(str[0]);
  this->setAction(str[1]);

  switch(this->action)
  {
    case MOTOREVENT__SET_PERCENT:
      this->type = SetPercent;
      this->value = SerLink::Utils::strToUint8(&str[2], 3);
      if(this->value > 100)
        this->value = 100;
      break;
    case MOTOREVENT__SET_DIRECTION:
      this->type = SetDirection;
      if(str[2] == MOTOREVENT__DIRECTION_FORWARD)
      {
        this->value = Motor::directionStates::Forward;
      }
      else if(str[2] == MOTOREVENT__DIRECTION_REVERSE)
      {
        this->value = Motor::directionStates::Reverse;
      }
      else if(str[2] == MOTOREVENT__DIRECTION_DISABLED)
      {
        this->value = Motor::directionStates::Disabled;
      }
      else
      {
        this->type = None;
        return false;
      }
      break;
    case MOTOREVENT__SET_FREQUENCY:
      this->type = SetFrequency;
      switch(str[2])
      {
        case MOTOREVENT__FREQUENCY_500_HZ:
          this->value = PWM_FREQ_500_HZ;
          break;
        case MOTOREVENT__FREQUENCY_1_KHZ:
          this->value = PWM_FREQ_1_KHZ;
          break;
        case MOTOREVENT__FREQUENCY_2_KHZ:
          this->value = PWM_FREQ_2_KHZ; 
          break;
        case MOTOREVENT__FREQUENCY_5_KHZ:
          this->value = PWM_FREQ_5_KHZ;
          break;
        case MOTOREVENT__FREQUENCY_10_KHZ:
          this->value = PWM_FREQ_10_KHZ;
          break;
        case MOTOREVENT__FREQUENCY_20_KHZ:
          this->value = PWM_FREQ_20_KHZ;
          break;
        default:
          this->type = None;
          return false;
      }
      break;
    default:
      this->type = None;
      return false;
  }
  return true;
}

void MotorEvent::clear()
{
  this->clr();         // base class clr()
  this->type = None;
  this->value = 0;
}

void MotorEvent::copy(Event* copyEvent)
{
  MotorEvent* copy = (MotorEvent*) copyEvent;
  copy->setAck(this->ack);
  copy->setId(this->getId());
  copy->setAction(this->action);
  copy->type = this->type;
  copy->value = this->value;
}
//-----------------------------------------------------------------------------------

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
          pwm0_init();
          pwm0_setFrequency(this->frequency);
          //pwm0_setDutyPercent(0);
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
          pwm0_init();
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

bool Motor::setFrequency(pwmFreqValues frequency)
{
  if(this->pwm == Motor::pwmTypes::PWM0)
  {
    pwm0_setFrequency(this->frequency);
    this->frequency = frequency;
    return true;
  }
  else if(this->pwm == Motor::pwmTypes::PWM1)
  {
    pwm1_setFrequency(this->frequency);
    this->frequency = frequency;
    return true;
  }
  return false;
}

pwmFreqValues Motor::getFrequency()
{
  return this->frequency;
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
