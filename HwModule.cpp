#include "HwModule.hpp"
#include "env.h"

namespace HardMod::Std
{
  
void HwModule::InitAdc(AdcPrescalerValues psValue)
{
  uint8_t mask = 0;
  switch(psValue)
  {
    case PS_2: { mask = 0; break; }
    case PS_4: { mask = 2; break; }
  }

  ADCSRA &= mask;
}

} // end namespace HardMod::Std