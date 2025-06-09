/*
 * Event.cpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */


#include "Event.hpp"

Event::Event(Event_defs::handler handler)
{
	this->handler = handler;
}

void Event::Invoke(void* pData)
{
	this->handler(pData);
}

