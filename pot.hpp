#ifndef POT_HPP
#define POT_HPP

#include "HardMod_Event.hpp"
#include "idChar.hpp"
#include "Adc.hpp"

#define POTEVENT__CHANGE 'C'

namespace HardMod::Std
{
class PotEvent: public Event, public VariableIdChar
{
  protected:
    uint8_t percent;

  public:
    PotEvent();
    void setPercent(uint8_t value);
    uint8_t getPercent();

    //--------------------
    // Over-ridden base class methods

    uint8_t serialise(char* str);

    void clear();

    void copy(Event* copyEvent);
    //--------------------
};

class Pot : public FixedIdChar
{
  protected:
    uint8_t buffer[2]; // percent buffer
    uint8_t bufferIndex;
    Adc* adc = nullptr;
    uint8_t adcIndex;
    int8_t percent;
    uint16_t startTick;

  public:
    Pot(char id, Adc* adc, uint8_t adcIndex);
    void run();
    int8_t getPercent();
    bool getEvent(PotEvent* event);
};

} // end namespace HardMod::Std

#endif // POT_HPP