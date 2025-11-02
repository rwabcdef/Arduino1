#include "pwm_common.h"

// ====================================================
// === PWM0 (Timer1 / pin10 / PB2 / OC1B) =============
// ====================================================

static uint16_t pwm0_top_value = 1999;

void pwm0_init(void)
{
  DDRB |= (1 << PB2); // PB2 = output (pin 10)

  // Fast PWM, TOP = ICR1, non-inverting on OC1B
  TCCR1A = (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // prescaler = 8
  ICR1 = pwm0_top_value;
  OCR1B = 0;
}

void pwm0_setFrequency(pwmFreqValues freq)
{
  uint32_t targetHz;
  uint16_t prescaler = 8;

  switch (freq)
  {
    case PWM_FREQ_500_HZ:  targetHz = 500;  break;
    case PWM_FREQ_1_KHZ:   targetHz = 1000; break;
    case PWM_FREQ_2_KHZ:   targetHz = 2000; break;
    case PWM_FREQ_5_KHZ:   targetHz = 5000; break;
    case PWM_FREQ_10_KHZ:  targetHz = 10000;break;
    case PWM_FREQ_20_KHZ:  targetHz = 20000;break;
    default:               targetHz = 1000; break;
  }

  pwm0_top_value = (F_CPU / (prescaler * targetHz)) - 1;
  ICR1 = pwm0_top_value;
}

void pwm0_setDutyPercent(uint8_t percent)
{
  if (percent > 100)
    percent = 100;

  OCR1B = ((uint32_t)pwm0_top_value * percent) / 100;
}

// ====================================================
// === PWM1 (Timer2 / pin3 / PD3 / OC2B) ==============
// ====================================================

static uint8_t pwm1_top_value = 255;

static uint8_t pwm1_prescalerBits(uint16_t prescaler)
{
  switch (prescaler) {
    case 1:    return (1 << CS20);
    case 8:    return (1 << CS21);
    case 32:   return (1 << CS21) | (1 << CS20);
    case 64:   return (1 << CS22);
    case 128:  return (1 << CS22) | (1 << CS20);
    case 256:  return (1 << CS22) | (1 << CS21);
    case 1024: return (1 << CS22) | (1 << CS21) | (1 << CS20);
    default:   return (1 << CS21);
  }
}

void pwm1_init(void)
{
  DDRD |= (1 << PD3); // PD3 = output (pin 3)

  // Fast PWM, TOP = OCR2A, non-inverting on OC2B
  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << WGM22) | (1 << CS21); // prescaler = 8 default
  OCR2A = pwm1_top_value;
  OCR2B = 0;
}

void pwm1_setFrequency(pwmFreqValues freq)
{
  uint32_t targetHz;
  uint16_t prescaler = 8;

  switch (freq)
  {
    case PWM_FREQ_500_HZ:  targetHz = 500;  prescaler = 64; break;
    case PWM_FREQ_1_KHZ:   targetHz = 1000; prescaler = 64; break;
    case PWM_FREQ_2_KHZ:   targetHz = 2000; prescaler = 32; break;
    case PWM_FREQ_5_KHZ:   targetHz = 5000; prescaler = 8;  break;
    case PWM_FREQ_10_KHZ:  targetHz = 10000;prescaler = 8;  break;
    case PWM_FREQ_20_KHZ:  targetHz = 20000;prescaler = 8;  break;
    default:               targetHz = 5000; prescaler = 8;  break;
  }

  uint32_t top = (F_CPU / (prescaler * targetHz)) - 1;
  if (top > 255) top = 255;
  if (top < 1)   top = 1;

  pwm1_top_value = (uint8_t)top;
  OCR2A = pwm1_top_value;
  TCCR2B = (1 << WGM22) | pwm1_prescalerBits(prescaler);
}

void pwm1_setDutyPercent(uint8_t percent)
{
  if (percent > 100)
    percent = 100;

  OCR2B = ((uint16_t)(pwm1_top_value + 1) * percent) / 100;
}
