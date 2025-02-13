/*
 * Transport.cpp
 *
 *  Created on: 22 Apr 2023
 *      Author: rw123
 */

#include<string.h>
#include "Transport.hpp"
#include "swTimer.h"

#define IDLE 0
#define TXWAIT 1
#define POST_TXWAIT 2

using namespace SerLink;

Transport::Transport(Reader* reader, Writer* writer, Frame* rxFrame, Frame* txFrame):
reader(reader), writer(writer), rxFrame(rxFrame), txFrame(txFrame)
{
  this->socketCount = 0;
  this->currentState = IDLE;
}

Socket* Transport::acquireSocket(char* protocol, Frame* rxFrame, Frame* txFrame,
		  uint16_t startRollCode, readHandler instantReadHandler)
{
  Socket* socket = nullptr;

  if(this->socketCount < (TRANSPORT_CONFIG__NUM_SOCKETS_MAX - 1))
  {
    // One or more sockets are free.
    socket = &this->socket[this->socketCount];
    this->socketCount++;

    // Initialise socket
    socket->init(protocol, rxFrame, txFrame, instantReadHandler, startRollCode);

    if(instantReadHandler == nullptr){ /* do nothing */}
    else
    {
      // Register instant ack callback (i.e. piggyback) hander
      this->reader->registerInstantCallback(socket->getProtocol(), instantReadHandler);
    }

  }
  else{ /* All sockets are already in use - so do nothing */ }

  return socket;
}

int8_t Transport::findSocketIndex(char* protocol)
{
  uint8_t i;

  for(i=0; i<this->socketCount; i++)
  {
    if(this->socket[i].getActive())
    {
      if(0 == strncmp(this->socket[i].getProtocol(), protocol, Frame::LEN_PROTOCOL))
      {
        // socket found.
        return i;
      }
    }
  }
  // socket not found.
  return -1;
}

// Common behaviour for all states.
void Transport::commonBehaviour()
{
#if defined(TRANSPORT_CONFIG__SOCKETS_ENABLED)

  if(this->reader->getRxFrame(this->rxFrame))
  {
    // A frame has been received
    int8_t socketIndex = this->findSocketIndex(this->rxFrame->protocol);

    if(socketIndex >= 0)
    {
      // socket found - pass it up to the relevant socket
      this->socket[socketIndex].setRxFrame(this->rxFrame);
    }
  }

#endif
}

void Transport::run()
{
  switch(this->currentState)
  {
    case IDLE: { this->currentState = this->idle(); break;}
    case TXWAIT: { this->currentState = this->txWait(); break;}
    case POST_TXWAIT: { this->currentState = this->postTxWait(); break;}
  }

  this->writer->run();
  this->reader->run();
}

//----------------------------------------------------------------
// start of state methods

uint8_t Transport::idle()
{
  uint8_t i;

  this->commonBehaviour();

  for(i=0; i<this->socketCount; i++)
  {
    if(this->socket[i].getActive())
    {
      if(this->socket[i].getTxFrame(this->txFrame))
      {
        this->writer->sendFrame(this->txFrame);
        return TXWAIT;
      }
    }
  }
  return IDLE;
}

uint8_t Transport::txWait()
{
  this->commonBehaviour();

  uint8_t status = this->writer->getStatus();
  if(status == SerLink::Writer::STATUS_BUSY)
  {
    return TXWAIT;
  }
  else
  {
    swTimer_tickReset(&this->startTick);
    return POST_TXWAIT;
  }
}

uint8_t Transport::postTxWait()
{
  this->commonBehaviour();

  if(swTimer_tickCheckTimeout(&this->startTick, 100))
  {
    return IDLE;
  }
  return POST_TXWAIT;
}
// end of state methods
//----------------------------------------------------------------
