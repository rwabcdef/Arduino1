#ifndef HWMODULE_HPP_
#define HWMODULE_HPP_

#include <stdint.h>
#include <stdbool.h>

namespace HardMod::Std
{
class HwModule
{
  public:
    enum AdcPrescalerValues{
      PS_2 = 0,
      PS_4,
      PS_8,
      PS_16,
      PS_32,
      PS_64,
      PS_128
    };

    enum AdcInputValues{
      ADC0 = 0,
      ADC1,
      ADC2,
      ADC3,
      ADC4,
      ADC5

    };
    
    static void InitAdc(AdcPrescalerValues psValue);

    static void enableAdc(bool enable);

    static void startAdcConversion();

    static bool isAdcConversionComplete();

    static void setAdcInput(AdcInputValues input);
};
} // end namespace HardMod::Std

#endif