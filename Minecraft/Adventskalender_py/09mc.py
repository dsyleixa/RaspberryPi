#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
LED = 18
k1 = 20

GPIO.setmode(GPIO.BCM)
GPIO.setup(LED, GPIO.OUT, initial=False)
GPIO.setup(k1, GPIO.IN)

try:
  while True:
    if GPIO.input(k1) == False:
      GPIO.output(LED, True)
      mc.setBlocks(3, 2, 4, 3, 2, 5, block.GOLD_ORE)
    else:
      GPIO.output(LED, False)
      mc.setBlocks(3, 2, 4, 3, 2, 5, block.COAL_ORE)
    time.sleep(0.05)

except KeyboardInterrupt:
  GPIO.cleanup()
