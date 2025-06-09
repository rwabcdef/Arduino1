/*
 * Event.hpp
 *
 *  Created on: 25 Jun 2023
 *      Author: rw123
 */

#ifndef EVENT_HPP_
#define EVENT_HPP_


#include <stdint.h>

namespace Event_defs{

typedef void (*handler)(void*);

}

class Event{

public:
	Event(Event_defs::handler handler);
	void Invoke(void* pData);

private:
	Event_defs::handler handler;
};


#endif /* EVENT_HPP_ */
