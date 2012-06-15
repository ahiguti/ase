
#### for IronPython

from System import Random

robj = Random(15)
r = 0
for i in range(1000000):
  r = robj.Next()
print r

