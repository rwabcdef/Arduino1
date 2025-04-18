import serial
import traceback

#---------------------------------------------------
class SerialPort:
  def __init__(self, port, baudrate):
    ser = serial.Serial('COM3', 19200, timeout=None)
    if ser.isOpen():
      ser.close()

  def init(self):
    # not finnished
    pass
    
#---------------------------------------------------

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
