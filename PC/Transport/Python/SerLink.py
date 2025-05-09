import serial
import traceback, time, threading, datetime , queue

# python SerLink.py
#---------------------------------------------------
class SerLink:
  def __init__(self, port, baudrate):
    self.debug = SerLink.Debug()
    self.port = SerLink.Port(port, baudrate)
    self.writer = SerLink.Writer(self.port, self.debug)
    self.reader = SerLink.Reader(self.port, self.writer, self.debug)

  def start(self):
    self.port.init()
    self.writerThread = threading.Thread(target=self.writer.run)
    self.writerThread.start()
    self.readerThread = threading.Thread(target=self.reader.run)
    self.readerThread.start()

  def close(self):
    self.writerThread.quit()
    self.writerThread.join()
    self.reader.quit()
    self.readerThread.join()
    self.port.close()

  def readline(self):
    return self.port.readline().decode("utf-8")
  
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
      s = '%s%s%s%s%s\n' % (self.protocol, self.type, self.rollCode, self.dataLen, self.data)
      return s

    def print(self):
      s = '%s, %s, %s, %s, %s' % (self.protocol, self.type, self.rollCode, self.dataLen, self.data)
      print(s)

    def sprint(self):
      s = '%s, %s, %s, %s, %s' % (self.protocol, self.type, self.rollCode, self.dataLen, self.data)
      return s

  class Port:
    def __init__(self, port, baudrate, timeout=1):
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
      self.ackWaitTime = ackWaitTime
      self.inputQueue = queue.Queue(maxsize=10)
      self.ackWait = False
      self.txFrame = None
      self.txStatus = None
      self.outputMutex = threading.Lock()
      self.outputMessage = SerLink.Writer.OutputMessage()

    def print(self, s):
      if(self.debug != None):
        self.debug.printer.print(s)
      else:
        print(s)

    def quit(self):
      msg = SerLink.Writer.InputMessage(SerLink.Writer.MSG_TYPE_QUIT)
      self.inputQueue.put(msg)

    def setAckFrame(self, frame):
      msg = SerLink.Writer.InputMessage(SerLink.Writer.MSG_TYPE_ACK_FRAME, data=frame)
      self.inputQueue.put(msg)

    def sendFrameWait(self, frame):
      msg = SerLink.Writer.InputMessage(SerLink.Writer.MSG_TYPE_TX_FRAME, data=frame)

      self.outputMutex.acquire()
      self.outputMessage.txStatus = SerLink.Writer.STATUS_BUSY
      self.outputMutex.release()

      self.inputQueue.put(msg)

      waitIteration = 0.005
      while(True):
        self.outputMutex.acquire()
        txStatus = self.outputMessage.txStatus
        self.outputMutex.release()

        if(txStatus == SerLink.Writer.STATUS_BUSY):
          time.sleep(waitIteration)
        else:
          break

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

        if(msg == None):
          # no new new frame to be sent or ack frame has been received

          if(self.ackWait == True):
            # We are currently waiting for an ack frame - but none has arrived within the timeout

            self.outputMutex.acquire()
            self.outputMessage.txStatus = SerLink.Writer.STATUS_TIMEOUT
            self.outputMessage.ackData = None
            self.outputMutex.release()
          else:
            # We are NOT currently waiting for an ack frame
            continue

        else:
          # process input message / received frame
          if(msg.msgType == SerLink.Writer.MSG_TYPE_QUIT):
            # quit thread
            break

          if(self.ackWait == True):
            # We are currently waiting for an ack frame

            if(msg.msgType == SerLink.Writer.MSG_TYPE_TX_FRAME):
              # We are currently waiting for an ack frame - so ignore new frame to be trasmitted
              continue

            elif(msg.msgType == SerLink.Writer.MSG_TYPE_ACK_FRAME):
              ackFrame = msg.data
              self.ackWait = False

              self.outputMutex.acquire()
              
              if(ackFrame.protocol == self.txFrame.protocol):
                # This IS the corresponding ack for the frame just sent

                

                if(ackFrame.dataLen < SerLink.Frame.ACK_OK):
                  self.outputMessage.txStatus = SerLink.Writer.STATUS_OK_DATA
                  self.outputMessage.ackData = ackFrame.data
                else:
                  self.outputMessage.txStatus = SerLink.Writer.STATUS_OK
                  self.outputMessage.ackData = None

              else:
                # This is NOT the corresponding ack for the frame just sent
                self.outputMessage.txStatus = SerLink.Writer.STATUS_TIMEOUT
                self.outputMessage.ackData = None

              self.outputMutex.release()
                
            else:
              continue
            
          else:
            # We are NOT currently waiting for an ack frame
            
            if(msg.msgType == SerLink.Writer.MSG_TYPE_TX_FRAME):
              # send frame
              self.txFrame = msg.data
              frameStr = self.txFrame.toString()
              self.port.writeLine(frameStr)

              if(self.txFrame.type == SerLink.Frame.TYPE_TRANSMISSION):
                # We DO expect an ack back
                self.ackWait = True
              else:
                # We do NOT expect an ack back
                self.ackWait = False

            elif(msg.msgType == SerLink.Writer.MSG_TYPE_ACK_FRAME):
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
      self.rxFrame = SerLink.Frame()
      self.debug = debug
      #self.ackFrame = SerLink.Frame()

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

        if(len(line) >= SerLink.Frame.LEN_HEADER + 1):
          self.rxFrame.fromString(line)
          self.print(self.rxFrame.sprint())

          if(self.rxFrame.type == SerLink.Frame.TYPE_TRANSMISSION):
            # send ack
            ackFrame = SerLink.Frame(self.rxFrame.protocol, SerLink.Frame.TYPE_ACK, self.rxFrame.rollCode, SerLink.Frame.ACK_OK)

            # ack send
            ackFrameStr = ackFrame.toString()
            self.port.write(ackFrameStr)

        elif(len(line) >= SerLink.Frame.LEN_ACK):

          if(self.rxFrame.type == SerLink.Frame.TYPE_ACK):
            # An ack frame has been received - so pass it to the writer
            self.writer.setAckFrame(self.rxFrame)

    def quit(self):
      self.quitFlag = True

  class Debug:
    def __init__(self):
      self.threadNameResolver = SerLink.Debug.ThreadNameResolver()
      self.timeStamper = SerLink.Debug.TimeStamper()
      self.printer = SerLink.Debug.Printer(self.threadNameResolver, self.timeStamper)

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

#---------------------------------------------------
# Run:
# python SerLink.py
if __name__ == '__main__':
  print('start 1')

  ser = SerLink('COM3', 19200)
  ret = ser.start()

  print("Enter a command:")
  
  while(1):
    inStr = input()
    print(inStr)
    if inStr == 'q':
      break
    # line = ser.readline()
    # print('rx> ' + line)

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
