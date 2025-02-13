#include<stdint.h>
#include"timer0.h"

volatile uint16_t g_timer0_ISR = 0;

ISR(TIMER0_COMPA_vect){//timer0 interrupt 1kHz
  g_timer0_ISR++;
}

void timer0_init()
{
    //set timer0 interrupt at 2kHz
    TCCR0A = 0;// set entire TCCR2A register to 0
    TCCR0B = 0;// same for TCCR2B
    TCNT0 = 0;//initialize counter value to 0
    // set compare match register for 2khz increments
    OCR0A = 248;// = (16*10^6) / (2000*64) - 1 (must be <256)
    // turn on CTC mode
    TCCR0A |= (1 << WGM01);
    // Set CS11 bit for 64 prescaler
    TCCR0B |= (1 << CS11) | (1 << CS10);
    // enable timer compare interrupt
    TIMSK0 |= (1 << OCIE0A);
}
