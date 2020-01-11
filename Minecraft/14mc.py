#!/usr/bin/python
import mcpi.minecraft as minecraft
import RPi.GPIO as GPIO
import random

mc = minecraft.Minecraft.create()
rgb1 = [25,24,23]
rgb2 = [5,7,8]
x = 0
z = 0

GPIO.setmode(GPIO.BCM)
for i in range(3):
  GPIO.setup(rgb1[i], GPIO.OUT, initial=False)
  GPIO.setup(rgb2[i], GPIO.OUT, initial=False)

try:
  while True:
    p = mc.player.getTilePos()
    if p.x != x:
      i = random.randrange(3)
      GPIO.output(rgb1[i],True)
      i = random.randrange(3)
      GPIO.output(rgb1[i],False)
      mc.postToChat("p.x:"+str(p.x)+" x:"+str(x)+" p.z:"+str(p.z)+" z:"+str(z))
      x = p.x
    if p.z != z:
      i = random.randrange(3)
      GPIO.output(rgb2[i],True)
      i = random.randrange(3)
      GPIO.output(rgb2[i],False)
      mc.postToChat("p.x:"+str(p.x)+" x:"+str(x)+" p.z:"+str(p.z)+" z:"+str(z))
      z = p.z

except KeyboardInterrupt:
  GPIO.cleanup()
