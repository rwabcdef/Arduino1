/*
 * InterruptSchedule.hpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#ifndef INTERRUPTSCHEDULE_HPP_
#define INTERRUPTSCHEDULE_HPP_


#include <list>
#include <stdint.h>
#include "Event.hpp"

typedef void (*InterruptRoutine)();

namespace InterruptSchedule_defs
{
enum Types
{
	Interval = 0,
	List,
	StringEvent
};
}

class InterruptSchedule
{
public:



	InterruptSchedule(InterruptRoutine routine, uint32_t interval, int32_t offset = 0);
	InterruptSchedule(InterruptRoutine routine, std::list<uint32_t>* triggerPoints_uS);
	InterruptSchedule(Event* pEvent, const char* str, std::list<uint32_t>* triggerPoints_uS);
  ~InterruptSchedule();
	void tickRun(const uint64_t tick);
	bool getDone();
	static InterruptSchedule* buildStringEvent(Event* pEvent, const char* str, uint32_t startTick, uint32_t tickInterval);

private:

	std::list<uint32_t>::iterator iter;
	bool done;
	uint32_t interval;
	uint32_t current; // used for current tick (Interval type) or current trigger point (List type)
	InterruptSchedule_defs::Types type;
	InterruptRoutine routine;
	std::list<uint32_t>* triggerPoints_uS;
	Event* pEvent;
	const char* str;
	uint16_t charIndex;

	InterruptSchedule();
};


#endif /* INTERRUPTSCHEDULE_HPP_ */
