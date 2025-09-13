#ifndef POTMODULE_HPP_
#define POTMODULE_HPP_

#include <stdint.h>
#include <stdbool.h>

namespace HardMod::Std
{
class PotModule
{
  protected:
    uint8_t input;
    uint16_t startTick;
    uint16_t rawValue;
    uint8_t percentValue;

  public:
    PotModule(uint8_t input);
};
} // end namespace HardMod::Std

#endif