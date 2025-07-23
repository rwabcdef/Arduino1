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
    case PS_8: { mask = 3; break; }
    case PS_16: { mask = 4; break; }
    case PS_32: { mask = 5; break; }
    case PS_64: { mask = 6; break; }
    case PS_128: { mask = 7; break; }
  }

  ADCSRA |= mask;

  // Enable ADC
  bitSet(ADCSRA, ADEN);

  // Set ADC voltage ref to AVCC with external capacitor at AREF pin
  bitSet(ADMUX, REFS0);
}

void HwModule::setAdcInput(AdcInputValues input)
{
  uint8_t value = (uint8_t) input;
  if(value <= 6)
  {
    ADMUX &= 0xf0; // clear lower 4 bits (MUX[0 -> 3])

    ADMUX |= value;
  }
}

void HwModule::startAdcConversion()
{
  bitSet(ADCSRA, ADSC);
}

bool HwModule::isAdcConversionComplete()
{
  if (bit_is_clear(ADCSRA, ADSC))
  {
    // conversion is complete
    return true;
  }
  else
  {
    // conversion is NOT complete
    return false;
  }
}

} // end namespace HardMod::Std