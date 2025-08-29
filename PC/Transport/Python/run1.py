import SerLink

#---------------------------------------------------------------------------  

# python3 run1.py
if __name__ == '__main__':

  cli = SerLink.App.Console.CLI()
  cli.start()

  while(True):
    cmd = cli.getCmd()
    if cmd == None:
      continue    
    elif cmd == 'q':
      print('quit')
      cli.close()
      break
    else:
      print('cmd[%d]: %s' % (len(cmd), cmd))

  print('end')