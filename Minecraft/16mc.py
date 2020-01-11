#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
GPIO.setmode(GPIO.BCM)
LED1 = [25,24,23]
LED2 = [5,7,8]
p = [0,0,0]
kr = 21
kg = 16
kb = 12
r = 0
g = 0
b = 0
x = 10
y = 3
z = [13,18,23]
c = [14,5,11,15]

for i in range(3):
  GPIO.setup(LED1[i], GPIO.OUT)
  GPIO.setup(LED2[i], GPIO.OUT)

for i in range(3):
  p[i] = GPIO.PWM(LED1[i], 50)
  p[i].start(0)

GPIO.setup(kr, GPIO.IN)
GPIO.setup(kg, GPIO.IN)
GPIO.setup(kb, GPIO.IN)

for i in range(3):
  mc.setBlocks(x,y,z[i],x,y,z[i]+1,block.WOOL.id, c[3])

try:
  while True:
    if GPIO.input(kr) == False:
      if r < 10:
        r += 1
      else:
        r = 0
      p[0].ChangeDutyCycle(r*10)
      mc.setBlocks(x,y,z[0],x,y,z[0]+1,block.WOOL.id, c[0])
      mc.setBlocks(x,y,z[1],x,y,z[1]+1,block.WOOL.id, c[3])
      mc.setBlocks(x,y,z[2],x,y,z[2]+1,block.WOOL.id, c[3])
      GPIO.output(LED2[0],True)
      time.sleep(0.1)
      GPIO.output(LED2[0],False)
    if GPIO.input(kg) == False:
      if g < 10:
        g += 1
      else:
        g = 0
      p[1].ChangeDutyCycle(g*10)
      mc.setBlocks(x,y,z[0],x,y,z[0]+1,block.WOOL.id, c[3])
      mc.setBlocks(x,y,z[1],x,y,z[1]+1,block.WOOL.id, c[1])
      mc.setBlocks(x,y,z[2],x,y,z[2]+1,block.WOOL.id, c[3])
      GPIO.output(LED2[1],True)
      time.sleep(0.1)
      GPIO.output(LED2[1],False)
    if GPIO.input(kb) == False:
      if b < 10:
        b += 1
      else:
        b = 0
      p[2].ChangeDutyCycle(b*10)
      mc.setBlocks(x,y,z[0],x,y,z[0]+1,block.WOOL.id, c[3])
      mc.setBlocks(x,y,z[1],x,y,z[1]+1,block.WOOL.id, c[3])
      mc.setBlocks(x,y,z[2],x,y,z[2]+1,block.WOOL.id, c[2])
      GPIO.output(LED2[2],True)
      time.sleep(0.1)
      GPIO.output(LED2[2],False)

except KeyboardInterrupt:
  for i in range(3):
    p[i].stop()
    mc.setBlocks(x,y,z[i],x,y,z[i]+1,block.AIR.id)
  GPIO.cleanup()
