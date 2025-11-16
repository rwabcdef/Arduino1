#ifndef PWM_COMMON_H
#define PWM_COMMON_H

#include "env.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PWM_FREQ_500_HZ = 0,
  PWM_FREQ_1_KHZ,
  PWM_FREQ_2_KHZ,
  PWM_FREQ_5_KHZ,
  PWM_FREQ_10_KHZ,
  PWM_FREQ_20_KHZ
} pwmFreqValues;

void pwm0_clr(void);
void pwm0_init(void);
void pwm0_setFrequency(pwmFreqValues freq);
void pwm0_setDutyPercent(uint8_t percent);
void pwm0_disable(void);
void pwm0_enable(void);

void pwm1_init(void);
void pwm1_setFrequency(pwmFreqValues freq);
void pwm1_setDutyPercent(uint8_t percent);
void pwm1_disable(void);
void pwm1_enable(void);

#ifdef __cplusplus
}
#endif

#endif // PWM_COMMON_H
