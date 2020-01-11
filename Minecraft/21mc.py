#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

t = 0.5

mc = minecraft.Minecraft.create()
LED = [18,24,27,4]
bl = 7 
k1 = 21
k2 = 16
k3 = 12

GPIO.setmode(GPIO.BCM)
for i in LED:
  GPIO.setup(i, GPIO.OUT, initial=False)
GPIO.setup(bl, GPIO.OUT, initial=False)
GPIO.setup(k1, GPIO.IN)
GPIO.setup(k2, GPIO.IN)
GPIO.setup(k3, GPIO.IN)

def haus0():
  mc.setBlocks(p.x-1, p.y, p.z, p.x-4, p.y+3, p.z-3, block.AIR.id)
  for i in LED:
    GPIO.output(i,False)

def haus1():
  mc.setBlocks(p.x-1, p.y, p.z, p.x-4, p.y+1, p.z-3, block.BRICK_BLOCK.id)
  mc.setBlocks(p.x-2, p.y, p.z-1, p.x-3, p.y, p.z-2, block.AIR.id)
  mc.setBlocks(p.x-1, p.y+1, p.z-1, p.x-3, p.y+1, p.z-2, block.AIR.id)
  mc.setBlocks(p.x-1, p.y+2, p.z-1, p.x-4, p.y+2, p.z-2, block.BRICK_BLOCK.id)
  mc.setBlocks(p.x-1, p.y+2, p.z, p.x-4, p.y+2, p.z, block.STAIRS_WOOD.id,3)
  mc.setBlocks(p.x-1, p.y+2, p.z-3, p.x-4, p.y+2, p.z-3, block.STAIRS_WOOD.id,2)
  mc.setBlocks(p.x-1, p.y+3, p.z-1, p.x-4, p.y+3, p.z-1, block.STAIRS_WOOD.id,3)
  mc.setBlocks(p.x-1, p.y+3, p.z-2, p.x-4, p.y+3, p.z-2, block.STAIRS_WOOD.id,2)
  GPIO.output(LED[0],True)

def haus2():
  mc.setBlocks(p.x-1, p.y, p.z, p.x-4, p.y+1, p.z-3, block.BRICK_BLOCK.id)
  mc.setBlocks(p.x-2, p.y, p.z-1, p.x-3, p.y, p.z-2, block.AIR.id)
  mc.setBlocks(p.x-2, p.y+1, p.z, p.x-3, p.y+1, p.z-2, block.AIR.id)
  mc.setBlocks(p.x-2, p.y+2, p.z, p.x-3, p.y+2, p.z-3, block.BRICK_BLOCK.id)
  mc.setBlocks(p.x-1, p.y+2, p.z, p.x-1, p.y+2, p.z-3, block.STAIRS_WOOD.id,1)
  mc.setBlocks(p.x-4, p.y+2, p.z, p.x-4, p.y+2, p.z-3, block.STAIRS_WOOD.id,0)
  mc.setBlocks(p.x-2, p.y+3, p.z, p.x-2, p.y+3, p.z-3, block.STAIRS_WOOD.id,1)
  mc.setBlocks(p.x-3, p.y+3, p.z, p.x-3, p.y+3, p.z-3, block.STAIRS_WOOD.id,0)
  GPIO.output(LED[1],True)

def haus3():
  mc.setBlocks(p.x-1, p.y, p.z, p.x-4, p.y+1, p.z-3, block.BRICK_BLOCK.id)
  mc.setBlocks(p.x-2, p.y, p.z-1, p.x-3, p.y, p.z-2, block.AIR.id)
  mc.setBlocks(p.x-2, p.y+1, p.z-1, p.x-4, p.y+1, p.z-2, block.AIR.id)
  mc.setBlocks(p.x-1, p.y+2, p.z-1, p.x-4, p.y+2, p.z-2, block.BRICK_BLOCK.id)
  mc.setBlocks(p.x-1, p.y+2, p.z, p.x-4, p.y+2, p.z, block.STAIRS_WOOD.id,3)
  mc.setBlocks(p.x-1, p.y+2, p.z-3, p.x-4, p.y+2, p.z-3, block.STAIRS_WOOD.id,2)
  mc.setBlocks(p.x-1, p.y+3, p.z-1, p.x-4, p.y+3, p.z-1, block.STAIRS_WOOD.id,3)
  mc.setBlocks(p.x-1, p.y+3, p.z-2, p.x-4, p.y+3, p.z-2, block.STAIRS_WOOD.id,2)
  GPIO.output(LED[2],True)

def haus4():
  mc.setBlocks(p.x-1, p.y, p.z, p.x-4, p.y+1, p.z-3, block.BRICK_BLOCK.id)
  mc.setBlocks(p.x-2, p.y, p.z-1, p.x-3, p.y, p.z-2, block.AIR.id)
  mc.setBlocks(p.x-2, p.y+1, p.z-1, p.x-3, p.y+1, p.z-3, block.AIR.id)
  mc.setBlocks(p.x-2, p.y+2, p.z, p.x-3, p.y+2, p.z-3, block.BRICK_BLOCK.id)
  mc.setBlocks(p.x-1, p.y+2, p.z, p.x-1, p.y+2, p.z-3, block.STAIRS_WOOD.id,1)
  mc.setBlocks(p.x-4, p.y+2, p.z, p.x-4, p.y+2, p.z-3, block.STAIRS_WOOD.id,0)
  mc.setBlocks(p.x-2, p.y+3, p.z, p.x-2, p.y+3, p.z-3, block.STAIRS_WOOD.id,1)
  mc.setBlocks(p.x-3, p.y+3, p.z, p.x-3, p.y+3, p.z-3, block.STAIRS_WOOD.id,0)
  GPIO.output(LED[3],True)

mc.postToChat("Position mit Sensor 1 festlegen")
try:
  while True:
    fertig = False
    d = 1
    if GPIO.input(k1)==False:
      p = mc.player.getTilePos()
      mc.postToChat("Haus mit Sensor 2 drehen oder entfernen")
      mc.postToChat("Haus mit Sensor 3 bauen")
      haus1()
      while fertig==False:
        GPIO.output(bl,True)
        if GPIO.input(k2)==False:
          d += 1
          if d > 4:
            d = 0
          if d == 1:
            haus0()
            haus1()
          elif d == 2:
            haus0()
            haus2()
          elif d == 3:
            haus0()
            haus3()
          elif d == 4:
            haus0()
            haus4()
          else:
            haus0()
          time.sleep(t)
        if GPIO.input(k3)==False:
          fertig = True
          for i in LED:
            GPIO.output(i,False)
          GPIO.output(bl,False)
          mc.postToChat("Position mit Sensor 1 festlegen")
    time.sleep(t)
    
except KeyboardInterrupt:
  GPIO.cleanup()
