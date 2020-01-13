#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import pyautogui, random, time, colorsys
import RPi.GPIO as GPIO

mc = minecraft.Minecraft.create()
LED = [27,24,18,4]
k = [16,21,12,7]
RGB1 = [26,19,13]
RGB2 = [6,5,11]
pwm1 = [0,0,0]
pwm2 = [0,0,0]

GPIO.setmode(GPIO.BCM)
for i in LED:
  GPIO.setup(i, GPIO.OUT, initial=False)
for i in k:
  GPIO.setup(i, GPIO.IN)
for i in RGB1:
  GPIO.setup(i, GPIO.OUT, initial=False)
for i in RGB2:
  GPIO.setup(i, GPIO.OUT, initial=False)
for i in range(3):
  pwm1[i] = GPIO.PWM(RGB1[i], 50)
  pwm1[i].start(0)
  pwm2[i] = GPIO.PWM(RGB2[i], 50)
  pwm2[i].start(0)

x1 = -6
x2 = 2
z1 = 9
z2 = 17
ende = False
x = random.randrange(x1,x2)
z = random.randrange(z1,z2)
mc.setBlocks(x1, 1, z1, x2, 1, z2, block.GRASS.id)
mc.setBlock(x, 0, z, block.DIAMOND_ORE.id)
mc.postToChat("Schatz versteckt")

while ende == False:
  p = mc.player.getTilePos()
  rgb = colorsys.hsv_to_rgb((abs(p.x-x)/(x2-x1)), 1, 1)
  for i in range(3):
    pwm1[i].ChangeDutyCycle(rgb[i])
  rgb = colorsys.hsv_to_rgb((abs(p.z-z)/(z2-z1)), 1, 1)
  for i in range(3):
    pwm2[i].ChangeDutyCycle(rgb[i])  
  if GPIO.input(k[0]) == False:
    pyautogui.keyDown('w')
    GPIO.output(LED[0],True)
  else:
    pyautogui.keyUp('w')
    GPIO.output(LED[0],False)
  if GPIO.input(k[1]) == False:
    pyautogui.keyDown('a')
    GPIO.output(LED[1],True)
  else:
    pyautogui.keyUp('a')
    GPIO.output(LED[1],False)
  if GPIO.input(k[2]) == False:
    pyautogui.keyDown('s')
    GPIO.output(LED[2],True)
  else:
    pyautogui.keyUp('s')
    GPIO.output(LED[2],False)
  if GPIO.input(k[3]) == False:
    pyautogui.keyDown('d')
    GPIO.output(LED[3],True)
  else:
    pyautogui.keyUp('d')
    GPIO.output(LED[3],False) 
  bl = mc.getBlock(x, 1, z)
  if bl == block.AIR.id:
    ende = True

mc.postToChat("Schatz gefunden")
for j in range(10):
  for i in LED:
    GPIO.output(i,True)
  time.sleep(0.1)
  for i in LED:
    GPIO.output(i,False)
  time.sleep(0.1)
for i in range(3):
  pwm1[i].stop()
  pwm2[i].stop()
mc.setBlocks(x1, 0, z1, x2, 1, z2, block.SAND.id)
GPIO.cleanup()
