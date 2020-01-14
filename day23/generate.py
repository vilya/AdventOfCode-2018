from random import randint

N = 10
LO = -1000
HI =  1000
RAD = 1000

for i in xrange(N):
  nanobot = (randint(LO, HI), randint(LO, HI), randint(LO, HI), randint(1, RAD))
  print "pos=<%d,%d,%d>, r=%d" % nanobot
