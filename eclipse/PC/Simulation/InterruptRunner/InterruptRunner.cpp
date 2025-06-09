/*
 * InterruptRunner.cpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#include "InterruptRunner.hpp"

#include <string.h>
#include <stdio.h>

//uint32_t InterruptRunner::hwTick = 0;
//char InterruptRunner::s[128] = {0};


InterruptRunner::InterruptRunner(DebugPrint* pDebugPrint)
{
	this->pDebugPrint = pDebugPrint;
	this->hwTick = 0;
}

InterruptRunner::~InterruptRunner()
{
  this->pSchedulers.clear();

  while(!this->pSchedulers.empty())
  {
    delete this->pSchedulers.front();
    this->pSchedulers.pop_front();
  }
  //printf("delete InterruptRunner\n");

}

void InterruptRunner::clear()
{
  this->pSchedulers.clear();
}

// Called by the SimClk object each tick, in order to run the required interrupt routines (ISRs).
void InterruptRunner::tickRun(const uint64_t tick)
{
	std::list<InterruptSchedule*> removeIist;

	for (this->iter = this->pSchedulers.begin(); this->iter != this->pSchedulers.end(); ++this->iter)
	{
		InterruptSchedule* current = (InterruptSchedule*) *this->iter;

		current->tickRun(tick);

		if(current->getDone())
		{
			removeIist.push_back(current);
		}
	}

	std::list<InterruptSchedule*>::iterator removeIter;
	for(removeIter = removeIist.begin(); removeIter != removeIist.end(); ++removeIter)
	{
		this->pSchedulers.remove((InterruptSchedule*) *removeIter);
	}

	/*
	if((tick % 1000) == 0)
	{
		//InterruptRunner::hwTick++;
		this->hwTick++;

		sprintf(this->s, "hwTick %d [%d]", this->hwTick, tick);
		//printf(s);
		this->pDebugPrint->writeLine(s, DebugPrint::One);

		//DebugPrint::debugPrint.
	}
	*/
}

void InterruptRunner::RegisterInterruptSchedule(InterruptSchedule* pSchedule)
{
	this->pSchedulers.push_back(pSchedule);
}


