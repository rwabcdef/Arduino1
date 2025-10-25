#include "Registers.h"

#if defined(ENV_CONFIG__SYSTEM_ARDUINO_UNO_R3)  

uint8_t readPortB(char* str)
{
  if (str == NULL)
    return 0;

  // Read the registers
  uint8_t ddr = DDRB;
  uint8_t port = PORTB;
  uint8_t pin = PINB;

  // Format into 6-character hex string: DDRB + PORTB + PINB
  // e.g., "A5B207" for DDRB=0xA5, PORTB=0xB2, PINB=0x07
  sprintf(str, "%02X%02X%02X", ddr, port, pin);

  str[6] = '\0'; // Null terminate for safety
  return 6;      // Length of returned data
}

#endif