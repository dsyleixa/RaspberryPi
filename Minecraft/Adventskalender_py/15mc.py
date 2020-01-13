#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time, colorsys

mc = minecraft.Minecraft.create()
GPIO.setmode(GPIO.BCM)
LED = [25,24,23]
p = [0,0,0]
h = 0
k1 = 12
k2 = 21
x = 10
y = 3
z = 14
c = [3,5,4,1,6]

for i in range(3):
  GPIO.setup(LED[i], GPIO.OUT)

for i in range(3):
  p[i] = GPIO.PWM(LED[i], 50)
  p[i].start(0)

GPIO.setup(k1, GPIO.IN)
GPIO.setup(k2, GPIO.IN)

mc.setBlock(x,y,z,block.WOOL.id,c[0])

try:
  while True:
    if GPIO.input(k1) == False and h<95:
      h += 5
    if GPIO.input(k2) == False and h>0:
      h -= 5
    rgb = colorsys.hsv_to_rgb(h/100, 1, 1)
    for i in range(3):
      p[i].ChangeDutyCycle(rgb[i])
    mc.setBlock(x,y,z,block.WOOL.id,c[int(h/20)])
    time.sleep(0.1)

except KeyboardInterrupt:
  for i in range(3):
    p[i].stop()
  mc.setBlock(x,y,z,block.AIR.id)
  GPIO.cleanup()
