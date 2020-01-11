#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO

mc = minecraft.Minecraft.create()
b = 18
LED = [20,16,12,7,8,23]
ein = [2,6,1,0,5,3]
aus = [10,14,12,4,13,11]

GPIO.setmode(GPIO.BCM)
GPIO.setup(b, GPIO.OUT, initial=False)
for i in LED:
  GPIO.setup(i, GPIO.OUT, initial=False)

mc.postToChat("Zuerst farbige Bloecke in die Fenster setzen")
try:
  while True:
    for hit in mc.events.pollBlockHits():
      b1 = mc.getBlockWithData(hit.pos.x, hit.pos.y, hit.pos.z)
      if b1.id == block.BRICK_BLOCK.id:
        b2 = mc.getBlockWithData(hit.pos.x, hit.pos.y+1, hit.pos.z)
        if b2.id == block.WOOL.id:
          d = b2.data
          for i in range(6):
            if d == aus[i]:
              mc.setBlock(hit.pos.x, hit.pos.y+1, hit.pos.z, block.WOOL.id, ein[i])
              GPIO.output(LED[i],True)
              GPIO.output(b,True)
          for i in range(6):
            if d == ein[i]:
              mc.setBlock(hit.pos.x, hit.pos.y+1, hit.pos.z, block.WOOL.id, aus[i])
              GPIO.output(LED[i],False)
              GPIO.output(b,False)
    
except KeyboardInterrupt:
  GPIO.cleanup()
