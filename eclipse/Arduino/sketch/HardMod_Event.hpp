/*
 * Event.hpp
 *
 *  Created on: 4 Jun 2024
 *      Author: rob
 */

#ifndef ARDMOD_EVENT_HPP_
#define ARDMOD_EVENT_HPP_
 
#include <stdint.h>
 
namespace HardMod
{
 
class Event{

  protected:
    char action;
 
  public:
 
    Event();

    // Sets the Event's action
    void setAction(char action);
 
    // Convert this event to a string. Returns the string length.
    virtual uint8_t serialise(char* str);
 
    // Parses the input string and sets the variables of this event accordingly.
    virtual bool deSerialise(char* str);
 
    // Copies the variables of this event into the other event (copyEvent).
    virtual void copy(Event* copyEvent);
 };

 //------------------------------------------------------------
 class HasUnitId{

  protected:
    char unitId;
  public:
    HasUnitId(char unitId);
    void setUnitId(char unitId);
 };

} // end namespace ArdMod



#endif /* ARDMOD_EVENT_HPP_ */