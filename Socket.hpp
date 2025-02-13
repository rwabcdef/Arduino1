/*
 * Socket.hpp
 *
 *  Created on: 18 Nov 2023
 *      Author: rw123
 */

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

//#include "Socket_config.hpp"
#include "Writer.hpp"
#include "Reader.hpp"
#include "StateMachine.hpp"
#include "Frame.hpp"
#include "DebugUser.hpp"

namespace SerLink
{

class Socket : public DebugUser
{
private:
  //Writer* writer;
  //Reader* reader;
  bool active;
  bool txFlag;
  //bool txBusy;
  bool rxFlag;
  uint8_t txStatus;
  readHandler instantReadHandler;
  Frame *rxFrame;
  Frame* txFrame;
  char protocol[Frame::LEN_PROTOCOL];
  uint16_t txRollCode;

public:
  const static uint8_t TX_STATUS_IDLE = 5;
  const static uint8_t TX_STATUS_BUSY = 6;

  //Socket(Writer* writer, Reader* reader);
  Socket();
  bool getActive(){ return this->active; };
  void init(char* protocol, Frame *rxFrame, Frame* txFrame, readHandler instantReadHandler, uint16_t startRollCode = 0);

  //-------------------------------------------
  // Upper (i.e. application) Interface

  // Gets received data from the socket (sent by the remote device).
  // Returns true if received data is ready to be read from the socket.
  bool getRxData(char* data, uint8_t* dataLen);

  // Sends data from the socket to the remote device.
  // Returns true if the data has been accepted.
  bool sendData(char* data, uint16_t dataLen, bool ack);

  // Returns the send status, and clears it.
  uint8_t getAndClearSendStatus();

  //-------------------------------------------
  // Lower (i.e. transport) Interface

  char* getProtocol(){ return &this->protocol[0]; };

  // Called by transport layer, to see if there is a frame to be sent from this Socket.
  bool getTxFrame(Frame* txFrame);

  // Called by transport layer when writer has finished sending frame
  void setWriterDoneStatus(uint8_t status);

  // Called by transport layer when reader has a frame for this Socket.
  void setRxFrame(Frame* rxFrame);
  //-------------------------------------------
};

} // namespace SerLink



#endif /* SOCKET_HPP_ */
