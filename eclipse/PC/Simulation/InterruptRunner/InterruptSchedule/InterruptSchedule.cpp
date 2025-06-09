/*
 * InterruptSchedule.cpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#include "InterruptSchedule.hpp"
#include <string.h>
#include <stdio.h>

InterruptSchedule::InterruptSchedule()
{
	this->done = false;
}

InterruptSchedule::InterruptSchedule(InterruptRoutine routine, uint32_t interval, int32_t offset)
: InterruptSchedule()
{
	this->type = InterruptSchedule_defs::Types::Interval;
	this->routine = routine;
	this->interval = interval;
	this->current = offset;
}

InterruptSchedule::InterruptSchedule(InterruptRoutine routine, std::list<uint32_t>* triggerPoints_uS)
: InterruptSchedule()
{
	this->type = InterruptSchedule_defs::Types::List;
	this->routine = routine;
	this->triggerPoints_uS = triggerPoints_uS;
	this->iter = this->triggerPoints_uS->begin();
	this->current = *this->iter;
}

InterruptSchedule::InterruptSchedule(Event* pEvent, const char* str, std::list<uint32_t>* triggerPoints_uS)
: InterruptSchedule()
{
	this->type = InterruptSchedule_defs::Types::StringEvent;
	this->charIndex = 0;
	this->pEvent = pEvent;
	this->str = str;
	this->triggerPoints_uS = triggerPoints_uS;
	this->iter = this->triggerPoints_uS->begin();
	this->current = *this->iter;
}

InterruptSchedule::~InterruptSchedule()
{
//  while(!this->triggerPoints_uS->empty())
//  {
//    delete &this->triggerPoints_uS->front();
//    this->triggerPoints_uS->pop_front();
//  }
  //this->triggerPoints_uS->clear();
}

void InterruptSchedule::tickRun(const uint64_t tick)
{
	switch(this->type)
	{
		case InterruptSchedule_defs::Types::Interval:
		{
			this->current++;

			if((this->current > 0) && ((this->current % this->interval) == 0))
			{
				this->routine();
			}

			break;
		}
		case InterruptSchedule_defs::Types::List:
		{
			if(!this->done)
			{
				if(tick == this->current)
				{
					this->routine();

					this->iter++;
					this->current = *this->iter;
				}
			}

			if(this->iter == this->triggerPoints_uS->end())
			{
				this->done = true;
			}

			break;
		}
		case InterruptSchedule_defs::Types::StringEvent:
			{
				if(!this->done)
				{
					if(tick == this->current)
					{
						this->pEvent->Invoke((void*) this->str[this->charIndex]);

						this->iter++;
						this->current = *this->iter;
						this->charIndex++;
					}
				}

				if(this->iter == this->triggerPoints_uS->end())
				{
					this->done = true;
				}

				break;
			}
	}
}

bool InterruptSchedule::getDone()
{
	return this->done;
}

InterruptSchedule* InterruptSchedule::buildStringEvent
(Event* pEvent, const char* str, uint32_t startTick, uint32_t tickInterval)
{
	uint32_t i, currentTick = startTick;
	uint32_t strLen = (uint32_t) strlen(str);

	std::list<uint32_t>* triggerPoints_uS = new std::list<uint32_t>();

	for(i=0; i<strLen; i++)
	{
		triggerPoints_uS->push_back(currentTick);

		currentTick += tickInterval;
	}

	InterruptSchedule* ret = new InterruptSchedule(pEvent, str, triggerPoints_uS);

	return ret;
}


