#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
t1 = 16
t2 = 8
LED = [18,23,25]

GPIO.setmode(GPIO.BCM)
for i in LED:
  GPIO.setup(i, GPIO.OUT, initial=False)
GPIO.setup(t1, GPIO.IN, GPIO.PUD_DOWN)
GPIO.setup(t2, GPIO.IN, GPIO.PUD_DOWN)

try:
  while True:
    if GPIO.input(t1)==True:
      p = mc.player.getTilePos()
      m = mc.getBlock(p.x, p.y-1, p.z)
      for i in range(3):
        GPIO.output(LED[i],True)
        if m == block.LEAVES.id:
          mc.setBlock(p.x+1+i, p.y-1, p.z, block.LEAVES)
        else:
          mc.setBlock(p.x+1+i, p.y, p.z, block.LEAVES)
        time.sleep(0.05)
        GPIO.output(LED[i],False)
      time.sleep(0.2)
    if GPIO.input(t2)==True:
      p = mc.player.getTilePos()
      m = mc.getBlock(p.x, p.y-1, p.z)
      for i in range(3):
        GPIO.output(LED[i],True)
        if m == block.LEAVES.id:
          mc.setBlock(p.x, p.y-1, p.z+1+i, block.LEAVES)
        else:
          mc.setBlock(p.x, p.y, p.z+1+i, block.LEAVES)
        time.sleep(0.05)
        GPIO.output(LED[i],False)
      time.sleep(0.2)

except KeyboardInterrupt:
  GPIO.cleanup()
