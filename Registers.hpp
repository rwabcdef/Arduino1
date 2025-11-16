#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

class Registers
{
public:
  // --------------------------------------------------
  // Base static helper methods
  // --------------------------------------------------
  static void setBit(volatile uint8_t &reg, uint8_t bit)
  {
    reg |= (1 << bit);
  }

  static void clearBit(volatile uint8_t &reg, uint8_t bit)
  {
    reg &= ~(1 << bit);
  }

  static bool readBit(volatile uint8_t &reg, uint8_t bit)
  {
    return (reg & (1 << bit)) != 0;
  }

  static void writeByte(volatile uint8_t &reg, uint8_t value)
  {
    reg = value;
  }

  static uint8_t readByte(volatile uint8_t &reg)
  {
    return reg;
  }

  // --------------------------------------------------
  // Generic readPort dispatcher — automatically selects the port
  // --------------------------------------------------
  static uint8_t readPort(char port, char *str)
  {
    switch (port)
    {
#if defined(DDRB)
    case 'B':
    case 'b':
      return PortB::Read(str);
#endif
#if defined(DDRC)
    case 'C':
    case 'c':
      return PortC::Read(str);
#endif
#if defined(DDRD)
    case 'D':
    case 'd':
      return PortD::Read(str);
#endif
    default:
      str[0] = '\0';
      return 0;
    }
  }

  // ======================================================
  // PORT B
  // ======================================================
  class PortB
  {
  public:
    static uint8_t Read(char *str)
    {
      sprintf(str, "%02X%02X%02X", DDRB, PORTB, PINB);
      str[6] = '\0';
      return 6;
    }

    class SetBit
    {
    public:
      static void DDRB_(uint8_t bit) { Registers::setBit(DDRB, bit); }
      static void PORTB_(uint8_t bit) { Registers::setBit(PORTB, bit); }
    };

    class ClearBit
    {
    public:
      static void DDRB_(uint8_t bit) { Registers::clearBit(DDRB, bit); }
      static void PORTB_(uint8_t bit) { Registers::clearBit(PORTB, bit); }
    };

    class Write
    {
    public:
      static void DDRB_(uint8_t value) { Registers::writeByte(DDRB, value); }
      static void PORTB_(uint8_t value) { Registers::writeByte(PORTB, value); }
    };

    class ReadReg
    {
    public:
      static uint8_t DDRB_() { return Registers::readByte(DDRB); }
      static uint8_t PORTB_() { return Registers::readByte(PORTB); }
      static uint8_t PINB_() { return Registers::readByte(PINB); }
    };
  };

  // ======================================================
  // PORT C
  // ======================================================
  class PortC
  {
  public:
    static uint8_t Read(char *str)
    {
      sprintf(str, "%02X%02X%02X", DDRC, PORTC, PINC);
      str[6] = '\0';
      return 6;
    }

    class SetBit
    {
    public:
      static void DDRC_(uint8_t bit) { Registers::setBit(DDRC, bit); }
      static void PORTC_(uint8_t bit) { Registers::setBit(PORTC, bit); }
    };

    class ClearBit
    {
    public:
      static void DDRC_(uint8_t bit) { Registers::clearBit(DDRC, bit); }
      static void PORTC_(uint8_t bit) { Registers::clearBit(PORTC, bit); }
    };

    class Write
    {
    public:
      static void DDRC_(uint8_t value) { Registers::writeByte(DDRC, value); }
      static void PORTC_(uint8_t value) { Registers::writeByte(PORTC, value); }
    };

    class ReadReg
    {
    public:
      static uint8_t DDRC_() { return Registers::readByte(DDRC); }
      static uint8_t PORTC_() { return Registers::readByte(PORTC); }
      static uint8_t PINC_() { return Registers::readByte(PINC); }
    };
  };

  // ======================================================
  // PORT D
  // ======================================================
  class PortD
  {
  public:
    static uint8_t Read(char *str)
    {
      sprintf(str, "%02X%02X%02X", DDRD, PORTD, PIND);
      str[6] = '\0';
      return 6;
    }

    class SetBit
    {
    public:
      static void DDRD_(uint8_t bit) { Registers::setBit(DDRD, bit); }
      static void PORTD_(uint8_t bit) { Registers::setBit(PORTD, bit); }
    };

    class ClearBit
    {
    public:
      static void DDRD_(uint8_t bit) { Registers::clearBit(DDRD, bit); }
      static void PORTD_(uint8_t bit) { Registers::clearBit(PORTD, bit); }
    };

    class Write
    {
    public:
      static void DDRD_(uint8_t value) { Registers::writeByte(DDRD, value); }
      static void PORTD_(uint8_t value) { Registers::writeByte(PORTD, value); }
    };

    class ReadReg
    {
    public:
      static uint8_t DDRD_() { return Registers::readByte(DDRD); }
      static uint8_t PORTD_() { return Registers::readByte(PORTD); }
      static uint8_t PIND_() { return Registers::readByte(PIND); }
    };
  };
  // ======================================================
  // PWM0 Registers
  // ======================================================
  class PWM0
  {
    public:
      static uint8_t Read(char *str)
      {
        sprintf(str, "%02X%02X%02X%02X", TCCR1A, TCCR1B, OCR1BH, OCR1BL);
        str[8] = '\0';
        return 8;
      }
  };
};

#endif // REGISTERS_HPP
