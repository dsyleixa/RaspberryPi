#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
LED = [16,12,7,8,23]
pi = 4
TON = [261,293,329,369,415]

GPIO.setmode(GPIO.BCM)
for i in LED:
  GPIO.setup(i, GPIO.OUT, initial=False)
GPIO.setup(pi, GPIO.OUT, initial=False)
p = GPIO.PWM(pi, 1)

x1 = 9
y1 = 2
z1 = 7
c1 = [3,5,4,1,6]
c0 = [15,0,15,0,15]

for i in range(5):
  mc.setBlock(x1, y1, z1+i, block.WOOL.id, c0[i])

try:
  while True:
    for hit in mc.events.pollBlockHits():
      bl = mc.getBlockWithData(hit.pos.x, hit.pos.y, hit.pos.z)
      for i in range(5):
        if hit.pos.x == x1 and hit.pos.z == z1+i:
          mc.setBlock(x1, y1, z1+i, block.WOOL.id, c1[i])
          GPIO.output(LED[i],True)
          p.ChangeFrequency(TON[i])
          p.start(1)
          time.sleep(0.2)
          mc.setBlock(x1, y1, z1+i, block.WOOL.id, c0[i])
          GPIO.output(LED[i],False)
          p.stop()

except KeyboardInterrupt:
  mc.setBlocks(x1, y1, z1, x1, y1, z1+4, block.AIR.id)
  p.stop()
  GPIO.cleanup()
