#!/usr/bin/python
import mcpi.minecraft as minecraft
import mcpi.block as block
import time, colorsys, subprocess
import RPi.GPIO as GPIO

mc = minecraft.Minecraft.create()
LED = [12,7,8,10,24]
RGB1 = [20,16,13]
RGB2 = [22,27,17]
pwm1 = [0,0,0]
pwm2 = [0,0,0]
b = 26
t1 = 21
t2 = 4

GPIO.setmode(GPIO.BCM)
for i in LED:
  GPIO.setup(i, GPIO.OUT, initial=False)
for i in RGB1:
  GPIO.setup(i, GPIO.OUT, initial=False)
for i in RGB2:
  GPIO.setup(i, GPIO.OUT, initial=False)
for i in range(3):
  pwm1[i] = GPIO.PWM(RGB1[i], 50)
  pwm1[i].start(0)
  pwm2[i] = GPIO.PWM(RGB2[i], 50)
  pwm2[i].start(0)
GPIO.setup(b, GPIO.OUT, initial=False)
GPIO.setup(t1, GPIO.IN, GPIO.PUD_DOWN)
GPIO.setup(t2, GPIO.IN, GPIO.PUD_DOWN)

x = -2
y = 1
z = 13
mc.setBlocks(x, y+1, z, x, y+2, z, block.WOOD.id)
mc.setBlocks(x-2, y+3, z-2, x+2, y+4, z+2, block.LEAVES.id)
mc.setBlocks(x-1, y+5, z-1, x+1, y+6, z+1, block.LEAVES.id)
mc.setBlocks(x, y+7, z, x, y+8, z, block.LEAVES.id)
sx = [x-2, x-2, x-2, x, x+2, x+2, x+2, x]
sz = [z-2, z, z+2, z+2, z+2, z, z-2, z-2]
for n in range(8):
  mc.setBlock(sx[n], y+1, sz[n], block.COAL_ORE.id)

def blink():
  GPIO.output(b,True)
  for k in range(3):
    for j in range(100):
      rgb = colorsys.hsv_to_rgb(j/100, 1, 1)
      for i in range(3):
        pwm1[i].ChangeDutyCycle(rgb[i])
      rgb = colorsys.hsv_to_rgb((100-j)/100, 1, 1)
      for i in range(3):
        pwm2[i].ChangeDutyCycle(rgb[i])
      GPIO.output(LED[j%5],True)
      if j%5 == 0:
        for n in range(8):
          mc.setBlock(sx[n], y+1, sz[n], block.COAL_ORE.id)
      if j%5 == 2:
        for n in range(8):
          mc.setBlock(sx[n], y+1, sz[n], block.GOLD_ORE.id)      
      time.sleep(0.1)
      GPIO.output(LED[j%5],False)
  GPIO.output(b,False)

try:
  while True:
    if GPIO.input(t1) == True:
      d = time.localtime()
      if d.tm_mon == 12 and d.tm_mday == 24:
        subprocess.Popen(["omxplayer", "Stille_Nacht_Klavier.mp3"])
        blink()
      else:
        mc.postToChat("Heute ist nicht Weihnachten")
    if GPIO.input(t2) == True:
      d = time.localtime()
      if d.tm_mon == 12 and d.tm_mday == 24:
        subprocess.Popen(["omxplayer", "O_Tannenbaum_Klavier.mp3"])
        blink()
      else:
        mc.postToChat("Heute ist nicht Weihnachten")
    time.sleep(0.2)

except KeyboardInterrupt:
  mc.setBlocks(x-2, y+1, z-2, x+2, y+8, z+2, block.AIR.id)
  for i in range(3):
    pwm1[i].stop()
    pwm2[i].stop()
  GPIO.cleanup()
