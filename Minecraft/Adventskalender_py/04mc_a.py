#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
print(mc)
LED = [18,23,25]

GPIO.setmode(GPIO.BCM)
for i in LED:
  GPIO.setup(i, GPIO.OUT, initial=False)

try:
  while True:    
    hits = mc.events.pollBlockHits()
    #if hits!=[]:
      #print(hits)
    for hit in hits:
      #print('my_Pos=',mc.player.getPos())
      mypos = mc.player.getPos()
      print('my_Pos=', "%4.1f, %4.1f, %4.1f" %(mypos.x, mypos.y, mypos.z))
      bl = mc.getBlockWithData(hit.pos.x, hit.pos.y, hit.pos.z)
      print('hitPos=', "%4.1f, %4.1f, %4.1f  ID=%3d" %(hit.pos.x, hit.pos.y, hit.pos.z, bl.id))
      if bl.id == block.GLOWSTONE_BLOCK.id:
        print('hit: glowstone=',bl.id)
        for i in LED:
          GPIO.output(i,True)
          time.sleep(0.1)
          GPIO.output(i,False)

except KeyboardInterrupt:
  GPIO.cleanup()
