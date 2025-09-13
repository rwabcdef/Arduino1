#include "HwModule.hpp"
#include "env.h"

namespace HardMod::Std
{
  
void HwModule::Adc_init(AdcPrescalerValues psValue)
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

void HwModule::Adc_setInput(AdcInputValues input)
{
  uint8_t value = (uint8_t) input;
  if(value <= 6)
  {
    ADMUX &= 0xf0; // clear lower 4 bits (MUX[0 -> 3])

    ADMUX |= value;
  }
}

void HwModule::Adc_startConversion()
{
  bitSet(ADCSRA, ADSC);
}

bool HwModule::Adc_isConversionComplete()
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

uint16_t HwModule::Adc_getResultRaw()
{
  return ADC;
}

uint8_t HwModule::Adc_getResultPercent()
{
  uint32_t res = ADC * 3;
  res += (res >> 4);
  res = (res >> 5);
  return (uint8_t) res;
}

} // end namespace HardMod::Std