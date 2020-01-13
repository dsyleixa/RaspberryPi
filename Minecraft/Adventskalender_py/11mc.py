#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
LED1 = 18
LED2 = 23
t1 = 8
t2 = 16

GPIO.setmode(GPIO.BCM)
GPIO.setup(LED1, GPIO.OUT, initial=False)
GPIO.setup(LED2, GPIO.OUT, initial=False)
GPIO.setup(t1, GPIO.IN, GPIO.PUD_DOWN)
GPIO.setup(t2, GPIO.IN, GPIO.PUD_DOWN)

try:
  while True:
    if GPIO.input(t1)==True:
      p = mc.player.getTilePos()
      mc.setBlock(p.x, p.y, p.z, block.LEAVES.id, 4)
      GPIO.output(LED1, True)
      time.sleep(0.1)
      GPIO.output(LED1, False)
    if GPIO.input(t2)==True:
      p = mc.player.getTilePos()
      mc.setBlock(p.x, p.y, p.z, block.LEAVES.id, 9)
      GPIO.output(LED2, True)
      time.sleep(0.1)
      GPIO.output(LED2, False)
        
except KeyboardInterrupt:
  GPIO.cleanup()
