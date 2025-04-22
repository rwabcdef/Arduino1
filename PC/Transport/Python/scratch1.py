#---------------------------------------------------
# Run:
# python SerLink.py
if __name__ == '__main__':
  print('start 1')

  ser = serial.Serial('COM3', 19200, timeout=None)
  ser.close()

  try: 
    ser.open()
  except Exception as e:
    print("error open serial port: " + str(e))
    ser.close()
    exit()
    

  if ser.isOpen():
    ser.flushInput()
    ser.flushOutput()

    while(1):
      line = ser.readline().decode("utf-8")
      print('rx> ' + line)

  ser.close()
  #------------------------------------------------------------------
  import serial
import traceback

#---------------------------------------------------
class SerLink:
  def __init__(self, port, baudrate):
    self.port = SerLink.Port(port, baudrate)

  def init(self):
    self.port.init()

  def close(self):
    self.port.close()

  def readline(self):
    return self.port.readline().decode("utf-8")

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
    
    def close(self):
      self.port.close()
    
#---------------------------------------------------
# Run:
# python SerLink.py
if __name__ == '__main__':
  print('start 1')

  ser = SerLink('COM3', 19200)
  ret = ser.init()

  while(1):
    line = ser.readline()
    print('rx> ' + line)

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
