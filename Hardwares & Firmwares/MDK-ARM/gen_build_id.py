#！/usr/bin/python

import sys
import os
import hashlib

md5 = hashlib.md5()
for root, dirs, files in os.walk(sys.argv[1], topdown=False):
    for name in files:
        if name != 'build_id.c' and (name.split('.')[-1] == 'c' 
            or name.split('.')[-1] == 'h'
            or name.split('.')[-1] == 's'
            or name.split('.')[-1] == 'py'):
            print("ADD %s"%(os.path.join(root, name)))
            with open (os.path.join(root, name), 'rb') as f:
                md5.update(f.read())
        else:
            print("SKIP %s"%(os.path.join(root, name)))

print("BUILD-ID: %s"%(md5.hexdigest()))

output = '#include "build_id.h"\n'
output += 'const uint8_t build_id[BUILD_ID_SIZE] = {'
for i in md5.digest(): output += "0x%02x,"%(i)
output += '};\n'

with open(sys.argv[2], 'w') as f:
    f.write(output);

exit(0)