#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO

mc = minecraft.Minecraft.create()
r = 25
g = 24
b = 23
r0 = 14
g0 = 13
b0 = 11
r1 = 1
g1 = 5
b1 = 3

GPIO.setmode(GPIO.BCM)
GPIO.setup(r, GPIO.OUT, initial=False)
GPIO.setup(g, GPIO.OUT, initial=False)
GPIO.setup(b, GPIO.OUT, initial=False)

mc.setBlock(9, 2, 7, block.WOOL.id, r0)
mc.setBlock(9, 2, 8, block.WOOL.id, g0)
mc.setBlock(9, 2, 9, block.WOOL.id, b0)

try:
  while True:
    for hit in mc.events.pollBlockHits():
      bl = mc.getBlockWithData(hit.pos.x, hit.pos.y, hit.pos.z)
      if bl.id == block.WOOL.id and bl.data == r0:
        mc.setBlock(9, 2, 7, block.WOOL.id, r1)
        GPIO.output(r, True)
      if bl.id == block.WOOL.id and bl.data == r1:
        mc.setBlock(9, 2, 7, block.WOOL.id, r0)
        GPIO.output(r, False)
      if bl.id == block.WOOL.id and bl.data == g0:
        mc.setBlock(9, 2, 8, block.WOOL.id, g1)
        GPIO.output(g, True)
      if bl.id == block.WOOL.id and bl.data == g1:
        mc.setBlock(9, 2, 8, block.WOOL.id, g0)
        GPIO.output(g, False)
      if bl.id == block.WOOL.id and bl.data == b0:
        mc.setBlock(9, 2, 9, block.WOOL.id, b1)
        GPIO.output(b, True)
      if bl.id == block.WOOL.id and bl.data == b1:
        mc.setBlock(9, 2, 9, block.WOOL.id, b0)
        GPIO.output(b, False)

except KeyboardInterrupt:
  GPIO.cleanup()
