#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
LED = [18,23,25]

GPIO.setmode(GPIO.BCM)
for i in LED:
  GPIO.setup(i, GPIO.OUT, initial=False)

try:
  while True:
    for hit in mc.events.pollBlockHits():
      bl = mc.getBlockWithData(hit.pos.x, hit.pos.y, hit.pos.z)
      if bl.id == block.GLOWSTONE_BLOCK.id:
        for i in LED:
          GPIO.output(i,True)
          time.sleep(0.05)
          GPIO.output(i,False)

except KeyboardInterrupt:
  GPIO.cleanup()
