#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
r = 18
o = 23
g = 24
t1 = 21
rf = 12
gf = 7
r1 = 14
o1 = 4
g1 = 5
sw = 15
ax = 9
az = 32
fx = 6
fz = 32

GPIO.setmode(GPIO.BCM)
GPIO.setup(r, GPIO.OUT, initial=False)
GPIO.setup(o, GPIO.OUT, initial=False)
GPIO.setup(g, GPIO.OUT, initial=True)
GPIO.setup(rf, GPIO.OUT, initial=True)
GPIO.setup(gf, GPIO.OUT, initial=False)
GPIO.setup(t1, GPIO.IN)

mc.setBlock(ax, 4, az, block.WOOL.id, sw)
mc.setBlock(ax, 3, az, block.WOOL.id, sw)
mc.setBlock(ax, 2, az, block.WOOL.id, g1)
mc.setBlock(fx, 3, fz, block.WOOL.id, r1)
mc.setBlock(fx, 2, fz, block.WOOL.id, sw)

try:
  while True:
    if GPIO.input(t1)==False:
      mc.setBlock(ax, 3, az, block.WOOL.id, o1)
      GPIO.output(o, True)
      mc.setBlock(ax, 2, az, block.WOOL.id, sw)
      GPIO.output(g, False)
      time.sleep(0.6)
      mc.setBlock(ax, 4, az, block.WOOL.id, r1)
      GPIO.output(r, True)
      mc.setBlock(ax, 3, az, block.WOOL.id, sw)
      GPIO.output(o, False)
      time.sleep(0.6)
      mc.setBlock(fx, 2, fz, block.WOOL.id, g1)
      GPIO.output(gf, True)
      mc.setBlock(fx, 3, fz, block.WOOL.id, sw)
      GPIO.output(rf, False)
      time.sleep(2)
      mc.setBlock(fx, 3, fz, block.WOOL.id, r1)
      GPIO.output(rf, True)
      mc.setBlock(fx, 2, fz, block.WOOL.id, sw)
      GPIO.output(gf, False)
      time.sleep(0.6)
      mc.setBlock(ax, 3, az, block.WOOL.id, o1)
      GPIO.output(o, True)
      time.sleep(0.6)
      mc.setBlock(ax, 2, az, block.WOOL.id, g1)
      GPIO.output(g, True)
      mc.setBlock(ax, 4, az, block.WOOL.id, sw)
      GPIO.output(r, False)
      mc.setBlock(ax, 3, az, block.WOOL.id, sw)
      GPIO.output(o, False)
      time.sleep(0.6)

except KeyboardInterrupt:
  mc.setBlock(ax, 2, az, block.WOOL.id, sw)
  mc.setBlock(fx, 3, fz, block.WOOL.id, sw)
  GPIO.cleanup()
        
    

        
