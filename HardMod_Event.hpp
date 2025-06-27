/*
 * Event.hpp
 *
 *  Created on: 4 Jun 2024
 *      Author: rob
 */

#ifndef HARDMOD_EVENT_HPP_
#define HARDMOD_EVENT_HPP_
 
#include <stdint.h>

#define EVENT__NONE '-' // code for: no event
 
namespace HardMod
{
 
class Event{

  protected:
    char action;
    void clr();
 
  public:
 
    Event();

    // Sets the Event's action
    void setAction(char action);

    char getAction();

    virtual char getActionClear();
 
    // Convert this event to a string. Returns the string length.
    virtual uint8_t serialise(char* str);
 
    // Parses the input string and sets the variables of this event accordingly.
    virtual bool deSerialise(char* str);
 
    // Copies the variables of this event into the other event (copyEvent).
    virtual void copy(Event* copyEvent);

    // Clear this event
    virtual void clear();
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