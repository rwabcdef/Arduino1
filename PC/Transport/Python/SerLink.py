import serial
import traceback, threading

#---------------------------------------------------
class SerLink:
  def __init__(self, port, baudrate):
    self.port = SerLink.Port(port, baudrate)
    self.reader = SerLink.Reader(self.port)

  def start(self):
    self.port.init()
    self.readerThread = threading.Thread(target=self.reader.run)
    self.readerThread.start()

  def close(self):
    self.reader.setQuit()
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

  class Port:
    def __init__(self, port, baudrate):
      self.port = serial.Serial(port, baudrate, timeout=None)
      
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
      return self.port.readline()
    
    def write(self, data):
      self.port.write(data)
    
    def close(self):
      self.port.close()
    
  class Reader:
    def __init__(self, port):
      self.port = port
      self.quit = False
      self.rxFrame = SerLink.Frame()
      #self.ackFrame = SerLink.Frame()

    def run(self):
      while(self.quit == False):
        line = self.port.readline().decode("utf-8")
        print('rx[%d]> %s' % (len(line), line))
        if(len(line) >= SerLink.Frame.LEN_HEADER + 1):
          self.rxFrame.fromString(line)
          self.rxFrame.print()

          if(self.rxFrame.type == SerLink.Frame.TYPE_TRANSMISSION):
            # send ack
            ackFrame = SerLink.Frame(self.rxFrame.protocol, SerLink.Frame.TYPE_ACK, self.rxFrame.rollCode, SerLink.Frame.ACK_OK)

            # ack send
            ackFrameStr = ackFrame.toString()
            self.port.write(ackFrameStr.encode('utf-8'))

    def setQuit(self):
      self.quit = True

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
