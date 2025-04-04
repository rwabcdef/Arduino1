/*
 * ATmega328.cpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#include<stdint.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ATmega328.hpp>

uint8_t UDR0 = 0;
uint8_t UCSR0A = 0;
uint8_t UCSR0B = 0;
uint8_t UCSR0C = 0;
uint8_t UBRR0L = 0;
uint8_t UBRR0H = 0;
uint8_t TCCR0A = 0;
uint8_t TCCR0B = 0;
uint8_t TCNT0 = 0;
uint8_t OCR0A = 0;
uint8_t TIMSK0 = 0;

uint8_t DDRB = 0;
uint8_t PORTB = 0;
uint8_t PINB = 0;

uint8_t DDRC = 0;
uint8_t PORTC = 0;
uint8_t PINC = 0;

uint8_t DDRD = 0;
uint8_t PORTD = 0;
uint8_t PIND = 0;

static setClearIntBitCallBack setUDRIE0CallBack = nullptr;
static setClearIntBitCallBack clrUDRIE0CallBack = nullptr;

volatile bool g_enable_USART_UDRE_ISR = false;

static uint16_t functionCode = 0;

static std::mutex mut;
static std::condition_variable cv;
static bool ready = false;

void enterCritical()
{
	std::unique_lock<std::mutex> lck(mut);
	while (!ready) cv.wait(lck);
}

void exitCritical(){
	std::unique_lock<std::mutex> lck(mut);
	ready = true;
	cv.notify_all();
}

void setFunctionCode(uint16_t code){
	functionCode = code;
}


void cbi(uint8_t reg, uint8_t bit)
{
	switch(bit)
	{
		case UDRIE0:
		{
			if(clrUDRIE0CallBack != nullptr)
			{
				clrUDRIE0CallBack();
			}
			break;
		}
	}
}
void sbi(uint8_t reg, uint8_t bit)
{
	switch(bit)
	{
		case UDRIE0:
		{
			if(setUDRIE0CallBack != nullptr)
			{
				setUDRIE0CallBack();
			}
			break;
		}
	}
}

void setSet_UDRIE0_CallBack(setClearIntBitCallBack cb)
{
	setUDRIE0CallBack = cb;
}

void setClr_UDRIE0_CallBack(setClearIntBitCallBack cb)
{
	clrUDRIE0CallBack = cb;
}


