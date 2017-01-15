#! /usr/bin/env python2
import os
import sys
from PIL import Image
import numpy as np

assert(len(sys.argv) == 5)
ims = (np.array(Image.open(os.path.join(sys.argv[1], "snapshot_{}.bmp".format(i)))) for i in range(int(sys.argv[2]), int(sys.argv[3])+1, int(sys.argv[4])))

s = np.zeros((384, 512, 3))
num = 0
for im in ims:
    s += im
    num += 1

print "average between {} pics.".format(num)

s = (s.astype(float) / num).astype(np.uint8)

result = Image.fromarray(s)
result.save('out.bmp')
