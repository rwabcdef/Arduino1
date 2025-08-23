from SerLink import Transport
from queue import Queue

class Car():
  def __init__(self, speed):
    self.speed = speed

class TestQueue(Queue): 
  def __init__(self):
    super().__init__()

  def getItem(self):
    return self.get()
  
  def taskDone(self):
    self.task_done()

  def putItem(self, item):
    self.put(item)

  def getSize(self):
    return self.qsize()
#---------------------------------------------------------------------------  
def test1():
  q = TestQueue()
  q.putItem(Car(6))
  q.putItem(Car(3))

  print('q size: %d' % q.getSize())

  c = q.getItem()
  print('car speed: %d' % c.speed)
  print('q size: %d' % q.getSize())

def test2():
  d = dict()
  d['c1'] = Car(7)
  d['c3'] = Car(5)

  key = 'c1'
  c = d[key]
  if not c == None:
    print('car speed: %d' % c.speed)

  key = 'c2'
  if not key in d.keys():
    print('Key: %s not found' % key)
#---------------------------------------------------------------------------  

# python test1.py
if __name__ == '__main__':
  print('start 1')

  # ser = Transport('Dummy', 19200)
  # ser.debug.threadNameResolver.addCurrentThread('MAIN ')

  #test1()
  test2()
