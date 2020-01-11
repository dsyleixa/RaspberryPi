#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
t1 = 16
t2 = 8

GPIO.setmode(GPIO.BCM)
GPIO.setup(t1, GPIO.IN, GPIO.PUD_DOWN)
GPIO.setup(t2, GPIO.IN, GPIO.PUD_DOWN)

try:
  while True:
    if GPIO.input(t1)==True:
      p = mc.player.getTilePos()
      mc.setBlock(p.x+3, p.y, p.z, block.WOOD)
      mc.setBlocks(p.x+2, p.y+1, p.z-1, p.x+4, p.y+2, p.z+1, block.LEAVES)
      time.sleep(0.2)
    if GPIO.input(t2)==True:
      p = mc.player.getTilePos()
      mc.setBlocks(p.x+2, p.y, p.z-1, p.x+4, p.y+2, p.z+1, block.AIR)
      time.sleep(0.2)

except KeyboardInterrupt:
  GPIO.cleanup()
