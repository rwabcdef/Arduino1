/*
 * InterruptRunner.hpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#ifndef INTERRUPTRUNNER_HPP_
#define INTERRUPTRUNNER_HPP_

#include <thread>
#include <list>
#include <stdint.h>
#include "InterruptSchedule.hpp"
#include "DebugPrint.hpp"

class InterruptRunner
{
public:
	InterruptRunner(DebugPrint* pDebugPrint);
	~InterruptRunner();
	void tickRun(const uint64_t tick);
	void RegisterInterruptSchedule(InterruptSchedule* pSchedule);
	void clear();

private:
	uint32_t hwTick;
	char s[128];
	std::list<InterruptSchedule*> pSchedulers;
	std::list<InterruptSchedule*>::iterator iter;
	DebugPrint* pDebugPrint;
};



#endif /* INTERRUPTRUNNER_HPP_ */
