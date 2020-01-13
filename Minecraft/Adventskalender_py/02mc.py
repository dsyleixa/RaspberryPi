#!/usr/bin/python
import mcpi.minecraft as minecraft
import RPi.GPIO as GPIO

mc = minecraft.Minecraft.create()
GPIO.setmode(GPIO.BCM)
GPIO.setup(23, GPIO.OUT)
GPIO.setup(25, GPIO.OUT)

while True:
  p = mc.player.getTilePos()
  if (p.x==8 and p.z>=3 and p.z <=4):
    GPIO.output(23, True)
    GPIO.output(25, False)
  else:
    GPIO.output(25, True)
    GPIO.output(23, False)
        
