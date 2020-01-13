#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

t = 1.0

mc = minecraft.Minecraft.create()
LED = [16,12,7,8,23]
t1 = 21
t2 = 4

GPIO.setmode(GPIO.BCM)
for i in LED:
  GPIO.setup(i, GPIO.OUT, initial=False)
GPIO.setup(t1, GPIO.IN, GPIO.PUD_DOWN)
GPIO.setup(t2, GPIO.IN, GPIO.PUD_DOWN)

try:
  while True:
    if GPIO.input(t1)==True:
      p = mc.player.getTilePos()
      mc.postToChat(str(p.x-1)+", "+str(p.y)+", "+str(p.z)+" ... "+str(p.x-4)+", "+str(p.y+3)+", "+str(p.z-3))
      mc.setBlocks(p.x-1, p.y, p.z, p.x-4, p.y+1, p.z-3, block.BRICK_BLOCK.id)
      GPIO.output(LED[0],True)
      time.sleep(t)
      mc.setBlocks(p.x-2, p.y, p.z-1, p.x-3, p.y, p.z-2, block.AIR.id)
      mc.setBlocks(p.x-1, p.y+1, p.z-1, p.x-3, p.y+1, p.z-2, block.AIR.id)
      GPIO.output(LED[1],True)
      time.sleep(t)
      mc.setBlocks(p.x-1, p.y+2, p.z-1, p.x-4, p.y+2, p.z-2, block.BRICK_BLOCK.id)
      GPIO.output(LED[2],True)
      time.sleep(t)
      mc.setBlocks(p.x-1, p.y+2, p.z, p.x-4, p.y+2, p.z, block.STAIRS_WOOD.id,3)
      mc.setBlocks(p.x-1, p.y+2, p.z-3, p.x-4, p.y+2, p.z-3, block.STAIRS_WOOD.id,2)
      GPIO.output(LED[3],True)
      time.sleep(t)
      mc.setBlocks(p.x-1, p.y+3, p.z-1, p.x-4, p.y+3, p.z-1, block.STAIRS_WOOD.id,3)
      mc.setBlocks(p.x-1, p.y+3, p.z-2, p.x-4, p.y+3, p.z-2, block.STAIRS_WOOD.id,2)
      GPIO.output(LED[4],True)
      time.sleep(t)
      for i in LED:
        GPIO.output(i,False)
    if GPIO.input(t2)==True:
      mc.postToChat(str(p.x-1)+", "+str(p.y)+", "+str(p.z)+" ... "+str(p.x-4)+", "+str(p.y+3)+", "+str(p.z-3))
      mc.setBlocks(p.x-1, p.y, p.z, p.x-4, p.y+3, p.z-3, block.AIR.id)
    time.sleep(t)
    
except KeyboardInterrupt:
  GPIO.cleanup()
