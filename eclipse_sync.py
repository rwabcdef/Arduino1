# Copies source files from Arduino IDE sketch dir to eclipse source dir.

import os, shutil

cwd = os.getcwd()
print('cwd: %s\n', cwd)
dest = os.path.join(cwd, 'eclipse\Arduino\sketch')
print('dest: %s\n', dest)

for file in os.listdir(cwd):
  if ((file.endswith(".cpp")) or (file.endswith(".c")) or (file.endswith(".hpp")) or (file.endswith(".h"))):
    #print(file)
    if(file == 'env_config.h'):
      continue
    shutil.copy2(file, dest)