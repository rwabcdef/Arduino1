import threading, datetime, time

#startDateTime = 0
mainThreadId = 0

def resoveThreadId():
  currentThreadId = threading.current_thread().ident
  if(currentThreadId == mainThreadId):
    return 'MAIN'
  else:
    return 'OTHER'

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
  
class DebugPrinter(object):
  def __init__(self):
    self.timeStamper = TimeStamper()
    self.threadKeyVal = dict()

  def addKeyVal(self, key, value):
    dict[key] = value

if __name__ == '__main__':
  #startDateTime = datetime.datetime.now()
  timeStamper = TimeStamper()
  mainThreadId = threading.current_thread().ident

  print('main start 1')

  time.sleep(2.063)

  #now = datetime.datetime.now()
  #elapsed = datetime.datetime.now() - startDateTime
  #print('elapsed: %d.%d' % (elapsed.seconds, elapsed.microseconds / 1000))
  #threadId = threading.current_thread().ident

  print('elapsed[%s]: %s' % (resoveThreadId(), timeStamper.getElapsed()))

  time.sleep(1.128)
  print('elapsed[%s]: %s' % (resoveThreadId(), timeStamper.getElapsed()))