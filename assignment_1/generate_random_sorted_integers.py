#!/usr/bin/env python3

import random

for f in (open('source1', 'wb'), open('source2', 'wb')):
    l = [random.randint(0, 2**32-1) for x in range(32)]
    l.sort()
    for x in l:
        f.write(x.to_bytes(4, 'little'))
    else:
        f.close()
