#！/usr/bin/python

import sys
import os
import hashlib

md5 = hashlib.md5()
for root, dirs, files in os.walk(sys.argv[1], topdown=False):
    for name in files:
        if name != 'build_id.c' and (name.split('.')[-1] == 'c' or name.split('.')[-1] == 'h'):
            with open (os.path.join(root, name), 'rb') as f:
                md5.update(f.read())

print('#include "build_id.h"')
print('const uint8_t build_id[16] = {')

for i in md5.digest(): print("0x%02x,"%i, end='')

print('};')

exit(0)