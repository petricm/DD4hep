#!/usr/bin/env python
import re
from itertools import izip_longest

def rewrite(line, args):
  line = line.replace("\"","").lstrip()
  args = [s.replace(".c_str()","") for s in args]
  line = re.split("%[-a-zA-Z_0-9]*",line)
  args = map(str.strip, args)
  line = ["\"" + sub for sub in line]
  line = [sub + "\"" for sub in line]
  result = [item for slist in izip_longest(line, args) for item in slist if item is not None]
  result = " << ".join(result)+";"
  result = result.replace("<< \"\" <<","<<")
  result = result.replace(" << \"\";",";")
  return result

def convert(line):
  line = line.replace(");","")
  line = re.sub("[+]*","",line)
  line = line.split(',')
  line[0]=line[0].replace("printout","LOG")+") << "
  del line[1]
  if len(line)==2:
    return ''.join(line).replace(".c_str()","").rstrip() + ";"
  return line[0]+ rewrite(line[1],line[2:])

filepath = 'DDCore/src/VolumeManager.cpp'
with open(filepath) as fp:
  line = fp.readline()
  line = line.rstrip('\n')
  
  cnt = 1
  while line:
    if "printout(" in line:
      while not line.strip().endswith(';'):
        line = line.rstrip()+fp.readline().rstrip().lstrip()
      print line.rstrip()
      line = convert(line)
      print line.rstrip()
    line = fp.readline()
    cnt += 1
