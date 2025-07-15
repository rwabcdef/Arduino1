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
    
    static void InitAdc(AdcPrescalerValues psValue);

    static void enable(bool enable);
};
} // end namespace HardMod::Std

#endif