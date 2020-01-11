#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
t1 = 8

GPIO.setmode(GPIO.BCM)
GPIO.setup(t1, GPIO.IN, GPIO.PUD_DOWN)

try:
  while True:
    if GPIO.input(t1)==True:
      p = mc.player.getTilePos()
      mc.setBlocks(p.x-1, p.y, p.z-1, p.x+1, p.y, p.z+1, block.SAND.id)
      mc.player.setPos(p.x, p.y+1, p.z)
      time.sleep(0.2)

except KeyboardInterrupt:
  GPIO.cleanup()
