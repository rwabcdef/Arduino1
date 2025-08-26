import serial
import traceback, time, threading, datetime , queue

# python SerLink.py
#---------------------------------------------------
class Socket:
  def __init__(self, parent, protocol, initialRollCode=0, onReceive=None):
    self.parent = parent
    self.protocol = protocol
    self.rollCode = initialRollCode
    self.onReceive = onReceive
    self.sendResponseEvent = threading.Event(time=2)
    self.sendAckFrame = None

  def sendDataWait(self, data, ack=True):
    if ack:
      frameType = Transport.Frame.TYPE_TRANSMISSION
      self.sendAckFrame = None # clear ack frame
    else:
      frameType = Transport.Frame.TYPE_UNIDIRECTION
    dataLen = len(data)
    txFrame = Transport.Frame(self.protocol, frameType, self.rollCode, dataLen, data)
    self.parent.sendFrameWait(txFrame)

    # wait for transport layer to send frame, and for ack to be returned if ack = True
    self.sendResponseEvent.wait()

    if ack:
      if self.sendAckFrame == None:
        # error: ack frame shold be set
        pass
      else:
        # check tx and ack frame roll codes match
        pass
    else:
      # We are not waiting for an ack - so do nothing
      pass

    self.sendResponseEvent.clear()

  # Used by transport layer to signal that a send operation has completed
  # set the ack frame after a data send
  def sendDone(self, ackFrame=None):
    self.sendAckFrame = ackFrame
    self.sendResponseEvent.set()


class SerLink:
  def __init__(self, port, baudrate):
    self.transport = Transport(port, baudrate, self)
    self.sockets = dict()  # dictionary of sockets

  def start(self):
    self.transport.start()

  def close(self):
    self.transport.close()

  def acquireSocket(self, protocol, initialRollCode=0):
    socket = Socket(protocol, initialRollCode)
    self.sockets[protocol] = socket
    return socket
  
  def sendFrameWait(self, frame):
    protocol = frame.protocol
    rollCode = frame.rollCode
    ret = self.transport.sendFrameWait(frame)

  # Used by transport layer to signal that a send operation has completed
  # set the ack frame if one is returned
  def sendDone(self, protocol, ackFrame=None):
    if protocol in self.sockets.keys():
      # the protocol has been found in self.sockets
      socket = self.sockets[protocol]
      socket.sendDone(ackFrame)
    else:
      # the protocol has NOT been found in self.sockets - error
      pass


class Transport:
  def __init__(self, port, baudrate, parent):
    self.debug = Transport.Debug()
    self.port = Transport.Port(port, baudrate)
    self.parent = parent
    self.writer = Transport.Writer(self.port, debug=self.debug, ackWaitTime=1)
    self.reader = Transport.Reader(self.port, writer=self.writer, debug=self.debug)

  def start(self):
    self.port.init()
    self.writerThread = threading.Thread(target=self.writer.run)
    self.writerThread.start()
    self.readerThread = threading.Thread(target=self.reader.run)
    self.readerThread.start()

  def close(self):
    self.writer.quit()
    self.writerThread.join()
    self.reader.quit()
    self.readerThread.join()
    self.port.close()

  def readline(self):
    return self.reader.readline()
  
  def sendFrameWait(self, frame):
    ret = self.writer.sendFrameWait(frame)
    return ret
  
  class Frame:
    TYPE_TRANSMISSION = 'T'
    TYPE_UNIDIRECTION = 'U'
    TYPE_ACK = 'A'
    LEN_PROTOCOL = 5
    LEN_TYPE = 1
    LEN_ROLLCODE = 3
    LEN_DATALEN = 3
    LEN_HEADER = LEN_PROTOCOL + LEN_TYPE + LEN_ROLLCODE + LEN_DATALEN
    LEN_ACK = LEN_PROTOCOL + LEN_TYPE + LEN_ROLLCODE
    ACK_OK = 900

    def __init__(self, protocol=None, type=None, rollCode=None, dataLen=None, data=None):
      self.protocol = protocol
      self.type = type
      self.rollCode = rollCode
      self.dataLen = dataLen
      self.data = data

      # self.TYPE_TRANSMISSION = 'T'
      # self.TYPE_UNIDIRECTION = 'U'
      # self.TYPE_ACK = 'A'
      # self.LEN_PROTOCOL = 5
      # self.LEN_TYPE = 1
      # self.LEN_ROLLCODE = 3
      # self.LEN_DATALEN = 3
      # self.LEN_HEADER = self.LEN_PROTOCOL + self.LEN_TYPE + self.LEN_ROLLCODE + self.LEN_DATALEN

    def fromString(self, str):
      start = 0
      end = self.LEN_PROTOCOL
      self.protocol = str[start:end]
      start = end
      end = end + self.LEN_TYPE
      self.type = str[start]
      start = end
      end = end + self.LEN_ROLLCODE
      self.rollCode = str[start:end]
      start = end
      end = end + self.LEN_DATALEN
      self.dataLen = str[start:end]
      start = end
      end = len(str)
      self.data = str[start:end]

    def toString(self):
      if(self.type == Transport.Frame.TYPE_ACK):
        s = '%s%s%s%s\n' % (self.protocol, self.type, self.rollCode, self.dataLen)
      else:
        if(self.data == None):
          s = '%s%s%s%s\n' % (self.protocol, self.type, Transport.Utils.padIntLeft(self.rollCode, Transport.Frame.LEN_ROLLCODE),
                              Transport.Utils.padIntLeft(self.dataLen, Transport.Frame.LEN_DATALEN))
        else:
          s = '%s%s%s%s%s\n' % (self.protocol, self.type, Transport.Utils.padIntLeft(self.rollCode, Transport.Frame.LEN_ROLLCODE),
                                Transport.Utils.padIntLeft(self.dataLen, Transport.Frame.LEN_DATALEN), self.data)
      return s

    def print(self):
      s = '%s, %s, %s, %s, %s' % (self.protocol, self.type, self.rollCode, self.dataLen, self.data)
      print(s)

    def sprint(self):
      s = '%s, %s, %s, %s, %s' % (self.protocol, self.type, self.rollCode, self.dataLen, self.data)
      return s

  class Port:
    def __init__(self, port, baudrate, timeout=1):
      if not port == 'Dummy':
        self.port = serial.Serial(port, baudrate, timeout=timeout)
      
    def init(self):
      if self.port.isOpen():
        self.port.close()
      try: 
        self.port.open()
      except Exception as e:
        print("error open serial port: " + str(e))
        self.port.close()
        return 1
    
      if self.port.isOpen():
        self.port.flushInput()
        self.port.flushOutput()
      return
    
    def readline(self):
      line = self.port.readline()
      if len(line) == 0:
        return ''
      else:
        return line.decode("utf-8")
    
    def writeLine(self, data):
      if(data[-1] != '\n'):
        data = data + '\n'
      print('port.writeLine: %s' % data)
      self.port.write(data.encode('utf-8'))
    
    def close(self):
      self.port.close()

  class Writer:

    MSG_TYPE_TX_FRAME = 1
    MSG_TYPE_ACK_FRAME = 2
    MSG_TYPE_QUIT = 3

    STATUS_BUSY = 21
    STATUS_OK = 50
    STATUS_OK_DATA = 53
    STATUS_TIMEOUT = 51
    STATUS_PROTOCOL_ERROR = 52

    class InputMessage:
      def __init__(self, msgType, data=None):
        self.msgType = msgType
        self.data = data    

    class OutputMessage:
      def __init__(self, txStatus=None, ackData=None):
        self.txStatus = txStatus
        self.ackData = ackData

    def __init__(self, port, debug=None, ackWaitTime=0.5):
      self.port = port
      self.debug = debug
      self.ackWaitTime = ackWaitTime
      self.inputQueue = queue.Queue(maxsize=10)
      self.ackWait = False
      self.txFrame = None
      self.txStatus = None
      self.outputMutex = threading.Lock()
      self.outputMessage = Transport.Writer.OutputMessage()

    def print(self, s):
      if(self.debug != None):
        self.debug.printer.print(s)
      else:
        print(s)

    def quit(self):
      msg = Transport.Writer.InputMessage(Transport.Writer.MSG_TYPE_QUIT)
      self.inputQueue.put(msg)

    def setAckFrame(self, frame):
      msg = Transport.Writer.InputMessage(Transport.Writer.MSG_TYPE_ACK_FRAME, data=frame)
      self.inputQueue.put(msg)

    def sendFrameWait(self, frame):
      self.print('Writer.sendFrameWait start: %s' % frame.toString()[:-1])

      msg = Transport.Writer.InputMessage(Transport.Writer.MSG_TYPE_TX_FRAME, data=frame)

      self.outputMutex.acquire()
      self.outputMessage.txStatus = Transport.Writer.STATUS_BUSY
      self.outputMutex.release()

      self.inputQueue.put(msg)

      waitIteration = 0.005
      while(True):
        self.outputMutex.acquire()
        txStatus = self.outputMessage.txStatus
        self.outputMutex.release()

        if(txStatus == Transport.Writer.STATUS_BUSY):
          time.sleep(waitIteration)
        else:
          break

      self.print('Writer.sendFrameWait end: %s' % str(self.outputMessage.txStatus))

      return self.outputMessage

    def run(self):
      if(self.debug != None):
        self.debug.threadNameResolver.addCurrentThread('WTR  ')

      while(True):
        msg = None
        try:
          msg = self.inputQueue.get(block=True, timeout=self.ackWaitTime)
        except queue.Empty as qe:
          # queue is empty - no message has been read from it with the ack wait time
          pass

        #self.print('Writer inputQueue timeout')

        if(msg == None):
          # no new new frame to be sent or ack frame has been received

          if(self.ackWait == True):
            # We are currently waiting for an ack frame - but none has arrived within the timeout

            self.outputMutex.acquire()
            self.outputMessage.txStatus = Transport.Writer.STATUS_TIMEOUT
            self.outputMessage.ackData = None
            self.outputMutex.release()
            self.ackWait = False
          else:
            # We are NOT currently waiting for an ack frame
            continue

        else:
          # process input message / received frame
          if(msg.msgType == Transport.Writer.MSG_TYPE_QUIT):
            # quit thread
            break

          if(self.ackWait == True):
            # We are currently waiting for an ack frame

            if(msg.msgType == Transport.Writer.MSG_TYPE_TX_FRAME):
              # We are currently waiting for an ack frame - so ignore new frame to be trasmitted
              # but re-add it to the input queue so that it is not lost, and can be sent later
              self.inputQueue.put(msg)

            elif(msg.msgType == Transport.Writer.MSG_TYPE_ACK_FRAME):
              ackFrame = msg.data
              self.ackWait = False

              self.print('Writer Rx Ack Frame: %s' % ackFrame.toString()[:-1])

              self.outputMutex.acquire()
              
              if(ackFrame.protocol == self.txFrame.protocol):
                # This IS the corresponding ack for the frame just sent

                

                if(int(ackFrame.dataLen) < Transport.Frame.ACK_OK):
                  # the ack frame contains data
                  self.outputMessage.txStatus = Transport.Writer.STATUS_OK_DATA
                  self.outputMessage.ackData = ackFrame.data
                else:
                  # the ack frame does not contains data
                  self.outputMessage.txStatus = Transport.Writer.STATUS_OK
                  self.outputMessage.ackData = None

              else:
                # This is NOT the corresponding ack for the frame just sent
                self.outputMessage.txStatus = Transport.Writer.STATUS_TIMEOUT
                self.outputMessage.ackData = None

              self.outputMutex.release()
                
            else:
              # not an ack frame - so dom nothing
              continue
            
          else:
            # We are NOT currently waiting for an ack frame
            
            if(msg.msgType == Transport.Writer.MSG_TYPE_TX_FRAME):
              # send frame

              self.txFrame = msg.data

              self.print('Writer Tx Frame start: %s' % self.txFrame.toString()) # [:-1]
              
              frameStr = self.txFrame.toString()
              self.port.writeLine(frameStr)

              self.print('Writer Tx Frame end')

              if(self.txFrame.type == Transport.Frame.TYPE_TRANSMISSION):
                # We DO expect an ack back
                self.ackWait = True
              else:
                # We do NOT expect an ack back
                self.ackWait = False

            elif(msg.msgType == Transport.Writer.MSG_TYPE_ACK_FRAME):
              # ignore ack frame - as we are not waiting for one
              continue

            else:
              continue

          # signal to inputQueue that message processing is complete
          self.inputQueue.task_done()
    
  class Reader:
    def __init__(self, port, writer, debug=None):
      self.port = port
      self.writer = writer
      self.quitFlag = False
      self.rxFrame = Transport.Frame()
      self.debug = debug
      #self.ackFrame = Transport.Frame()

    def print(self, s):
      if(self.debug != None):
        self.debug.printer.print(s)
      else:
        print(s)

    def run(self):
      if(self.debug != None):
        self.debug.threadNameResolver.addCurrentThread('RDR  ')

      while(self.quitFlag == False):
        line = self.port.readline()
        if len(line) == 0:
          continue

        #s = 'rx[%d]> %s' % (len(line), line)
        

        self.print('rx[%d]> %s' % (len(line), line))

        if(len(line) >= Transport.Frame.LEN_HEADER + 1):
          self.rxFrame.fromString(line)
          self.print(self.rxFrame.sprint())

          if(self.rxFrame.type == Transport.Frame.TYPE_TRANSMISSION):
            # send ack
            ackFrame = Transport.Frame(self.rxFrame.protocol, Transport.Frame.TYPE_ACK, self.rxFrame.rollCode, Transport.Frame.ACK_OK)

            # ack send
            ackFrameStr = ackFrame.toString()
            self.port.writeLine(ackFrameStr)

          elif(self.rxFrame.type == Transport.Frame.TYPE_ACK):
            # An ack frame has been received - so pass it to the writer
            self.writer.setAckFrame(self.rxFrame)

        #elif(len(line) >= Transport.Frame.LEN_ACK):

          

    def quit(self):
      self.quitFlag = True

  class Debug:
    def __init__(self):
      self.threadNameResolver = Transport.Debug.ThreadNameResolver()
      self.timeStamper = Transport.Debug.TimeStamper()
      self.printer = Transport.Debug.Printer(self.threadNameResolver, self.timeStamper)

    class ThreadNameResolver:
      def __init__(self):
        self.dict = dict()

      def addThreadName(self, threadId, threadName):
        self.dict[threadId] = threadName

      def addCurrentThread(self, threadName):
        threadId = threading.current_thread().ident
        self.dict[threadId] = threadName

      def getCurrentThreadName(self):
        if(threading.current_thread().ident in self.dict):
          return self.dict[threading.current_thread().ident]
        else:
          return '-----'
        
    class TimeStamper(object):
      def __init__(self):
        self.start = datetime.datetime.now()
      
      """
      Returns string containing elapsed time: seconds:milliseconds
      e.g.: 015.745
      """
      def getElapsed(self):
        elapsed = datetime.datetime.now() - self.start
        t1 = int(elapsed.microseconds / 1000)
        seconds = str(elapsed.seconds)
        milliseconds = str(t1).zfill(3)
        seconds = str(elapsed.seconds).zfill(3)

        s = '{seconds:s}.{milliseconds:s}'.format(seconds = seconds, milliseconds = milliseconds)
        #s = '{seconds:2d}.{milliseconds:3d}'.format(seconds = 2, milliseconds = 32)
        return s
  
    class Printer:
      def __init__(self, threadNameResolver, timeStamper):
        self.threadNameResolver = threadNameResolver
        self.timeStamper = timeStamper

      def print(self, s):
        print('[%s:%s] %s' % (self.threadNameResolver.getCurrentThreadName(), self.timeStamper.getElapsed(), s))

  class Utils:
    def padIntLeft(n, maxLen, padChar = '0'):
      s = str(n)
      if(len(s) >= maxLen):
        return s
      for i in range(0, maxLen - len(s)):
        s = padChar + s
      return s

#---------------------------------------------------
# ECHO1T076004abcd LED01T805003abc DBG01T156003ASD DBG01T156003aSD

# Run:
# python Transport.py
if __name__ == '__main__':
  print('start 1')

  ser = Transport('COM3', 19200)
  ser.debug.threadNameResolver.addCurrentThread('MAIN ')
  ret = ser.start()

  print("Enter a command:")
  
  while(1):
    inStr = input()
    print(inStr)
    if inStr == 'q':
      break
    # line = ser.readline()
    # print('rx> ' + line)

    if inStr == 't':
      # LED01T345004abcd
      txFrame = Transport.Frame("LED01", Transport.Frame.TYPE_TRANSMISSION, 345, 4, "abcd") 
      ret = ser.sendFrameWait(txFrame)
      print('txStatus: ' + str(ret.txStatus) + '    ack data: ' + str(ret.ackData))

    elif inStr == 'tn':
      # NOACKT956003byh
      txFrame = Transport.Frame("NOACK", Transport.Frame.TYPE_TRANSMISSION, 956, 3, "byh") 
      ret = ser.sendFrameWait(txFrame)
      print('txStatus: ' + str(ret.txStatus) + '    ack data: ' + str(ret.ackData))

    # ECHO1T076004abcd
    elif inStr == 'te':
      # NOACKT956003byh
      txFrame = Transport.Frame("ECHO1", Transport.Frame.TYPE_TRANSMISSION, 97, 4, "abcd") 
      ret = ser.sendFrameWait(txFrame)
      print('txStatus: ' + str(ret.txStatus) + '    ack data: ' + str(ret.ackData))

    #DBG01T156003ASD
    elif inStr == 'td1':
      txFrame = Transport.Frame("DBG01", Transport.Frame.TYPE_TRANSMISSION, 45, 3, "ASD") 
      ret = ser.sendFrameWait(txFrame)
      print('txStatus: ' + str(ret.txStatus) + '    ack data: ' + str(ret.ackData))

    elif inStr == 'tledon':
      txFrame = Transport.Frame("LED01", Transport.Frame.TYPE_TRANSMISSION, 45, 1, "1") 
      ret = ser.sendFrameWait(txFrame)
      print('txStatus: ' + str(ret.txStatus) + '    ack data: ' + str(ret.ackData))

    elif inStr == 'tledoff':
      txFrame = Transport.Frame("LED01", Transport.Frame.TYPE_TRANSMISSION, 45, 1, "0") 
      ret = ser.sendFrameWait(txFrame)
      print('txStatus: ' + str(ret.txStatus) + '    ack data: ' + str(ret.ackData))

  ser.close()

  # ser = serial.Serial('COM3', 19200, timeout=None)
  # ser.close()

  # try: 
  #   ser.open()
  # except Exception as e:
  #   print("error open serial port: " + str(e))
  #   ser.close()
  #   exit()
    

  # if ser.isOpen():
  #   ser.flushInput()
  #   ser.flushOutput()

  #   while(1):
  #     line = ser.readline().decode("utf-8")
  #     print('rx> ' + line)

  # ser.close()
