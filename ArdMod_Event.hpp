/*
 * Event.hpp
 *
 *  Created on: 4 Jun 2024
 *      Author: rob
 */

#ifndef ARDMOD_EVENT_HPP_
#define ARDMOD_EVENT_HPP_
 
#include <stdint.h>
 
 
namespace ArdMod
{
 
class Event{

  private:
    char action;
 
  public:
 
    Event();
 
    // Convert this event to a string. Returns the string length.
    virtual uint8_t serialise(char* str);
 
    // Parses the input string and sets the variables of this event accordingly.
    virtual bool deSerialise(char* str);
 
    // Copies the variables of this event into the other event (copyEvent).
    virtual void copy(Event* copyEvent);
 };

} // end namespace ArdMod



#endif /* ARDMOD_EVENT_HPP_ */