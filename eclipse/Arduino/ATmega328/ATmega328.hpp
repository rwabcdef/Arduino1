/*
 * ATmega328.hpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#ifndef ATMEGA328_HPP_
#define ATMEGA328_HPP_

#include<stdint.h>
#include<stdbool.h>

extern uint8_t UDR0;
extern uint8_t UCSR0A;
extern uint8_t UCSR0B;
extern uint8_t UCSR0C;
extern uint8_t UBRR0L;
extern uint8_t UBRR0H;
extern uint8_t TCCR0A;
extern uint8_t TCCR0B;
extern uint8_t TCNT0;
extern uint8_t OCR0A;
extern uint8_t TIMSK0;

extern uint8_t DDRB;
extern uint8_t PORTB;
extern uint8_t PINB;

extern uint8_t DDRC;
extern uint8_t PORTC;
extern uint8_t PINC;

extern uint8_t DDRD;
extern uint8_t PORTD;
extern uint8_t PIND;

#define PORTB0 0
#define PORTB1 1
#define PORTB2 2
#define PORTB3 3
#define PORTB4 4
#define PORTB5 5

#define PORTC0 0
#define PORTC1 1
#define PORTC2 2
#define PORTC3 3
#define PORTC4 4
#define PORTC5 5

#define PORTD0 0
#define PORTD1 1
#define PORTD2 2
#define PORTD3 3
#define PORTD4 4
#define PORTD5 5
#define PORTD6 6
#define PORTD7 7

#define UART_ISR_RX USART_RX_vect
#define UART_ISR_TX USART_UDRE_vect
#define TIMER0_ISR TIMER0_COMPA_vect

extern volatile bool g_enable_USART_UDRE_ISR;

#ifdef __cplusplus
extern "C" {
#endif

#define ISR(x) void x()   // declares interrupt routine(s)
#define RUN_ISR(x) x()    // runs an interrupt routine

void cbi(uint8_t reg, uint8_t bit);
void sbi(uint8_t reg, uint8_t bit);

void enterCritical();
void exitCritical();

#define cli() enterCritical()
#define sei() exitCritical()

void setFunctionCode(uint16_t code);

// declare interrupt routines
ISR(UART_ISR_RX);
ISR(UART_ISR_TX);
ISR(TIMER0_ISR);

typedef void (*setClearIntBitCallBack)();

void setSet_UDRIE0_CallBack(setClearIntBitCallBack cb);
void setClr_UDRIE0_CallBack(setClearIntBitCallBack cb);

#ifdef __cplusplus
}
#endif



/*
#ifdef __cplusplus
//#  define ISR(vector)
    //extern "C" void vector (void)
#define ISR(x) void x()
#else
#define ISR(x) void x()
#endif

#define cbi(a, b)
#define sbi(a, b)
*/



#define MPCM0 0
#define U2X0 1
#define UPE0 2
#define DOR0 3
#define FE0 4
#define UDRE0 5
#define TXC0 6
#define RXC0 7

#define TXB80 8
#define RXB80 9
#define UCSZ02 10
#define TXEN0 11
#define RXEN0 12
#define UDRIE0 13
#define TXCIE0 14
#define RXCIE0 15

#define UCPOL0 16
#define UCSZ00 17
#define UCPHA0 18
#define UCSZ01 19
#define UDORD0 20
#define USBS0 21
#define UPM00 22
#define UPM01 23
#define UMSEL00 24
#define UMSEL01 25

#define WGM01 26

#define CS10 50
#define CS11 51
#define CS12 52
#define WGM12 53
#define WGM13 54
#define ICES1 56
#define ICNC1 57

#define TOIE0 60
#define OCIE0A 61
#define OCIE0B 62

//---------------------------------------------------------
//static const char ATMEGA328__REG_NAME_PORTD[] = "PORTD\0";
//#define ATMEGA328__REG_NAME_PORTD "PORTD\0"

//---------------------------------------------------------

#endif /* ATMEGA328_HPP_ */
