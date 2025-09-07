import SerLink
import re

#---------------------------------------------------------------------------  
def processCmd(cmd):
  cmd = cmd.strip()
  processLed(cmd, 'gr', green)
  processLed(cmd, 'rd', red)
  # if(cmd.startswith('gr')):
  #   token = cmd.split('gr')[1]
  #   print('token: ' + token)

  #   match = re.match('1+', token)
  #   if(match):
  #     print('match 1')
  #     green.on()

  #   else:
  #     match = re.match('0+', token)
  #     if(match):
  #       print('match 0')
  #       green.off()

def processLed(cmd, id, led):
  if(cmd.startswith(id)):
    token = cmd.split(id)[1]
    print('token: ' + token)

    match = re.match('1+', token)
    if(match):
      print('match 1')
      led.on()

    else:
      match = re.match('0+', token)
      if(match):
        print('match 0')
        led.off()


#---------------------------------------------------------------------------  

# python3 run1.py
if __name__ == '__main__':

  serialPort =  'COM3' # '/dev/ttyACM0'

  serLink = SerLink.SerLink(serialPort, 19200, debugOn=True)
  serLink.debug.threadNameResolver.addCurrentThread('MAIN ')
  serLink.start()

  ledSocket = serLink.acquireSocket("LED01", "LED01Sock", initialRollCode=361)
  buttonSocket = serLink.acquireSocket("BUT01", "BUT001Sock", initialRollCode=127)

  green = SerLink.App.Module.Led(ledSocket, 'G')
  red = SerLink.App.Module.Led(ledSocket, 'R')

  cli = SerLink.App.Console.CLI()
  cli.start()

  while(True):
    cmd = cli.getCmd()
    if cmd == None:
      pass    
    elif cmd == 'q':
      print('quit')
      cli.close()
      break
    else:
      print('cmd[%d]: %s' % (len(cmd), cmd))
      processCmd(cmd)

    buttonData = buttonSocket.getRxData()
    if not buttonData == None:
      print('Button data: ' + buttonData)

  print('main quit')
  #ser.close()
  serLink.quit()