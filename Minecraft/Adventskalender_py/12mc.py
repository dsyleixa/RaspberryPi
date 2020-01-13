#!/usr/bin/python
import mcpi.minecraft as minecraft
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
r = 25
g = 24
b = 23
k1 = 12
k2 = 21
x = 3
z = 8

GPIO.setmode(GPIO.BCM)
GPIO.setup(r, GPIO.OUT, initial=False)
GPIO.setup(g, GPIO.OUT, initial=False)
GPIO.setup(b, GPIO.OUT, initial=False)
GPIO.setup(k1, GPIO.IN)
GPIO.setup(k2, GPIO.IN)

rp = GPIO.PWM(r, 50)
gp = GPIO.PWM(g, 50)
bp = GPIO.PWM(b, 50)
rp.start(0)
gp.start(0)
bp.start(0)
pwm = 0

try:
  while True:
    p = mc.player.getTilePos()
    if p.x >= x-10 and p.x <= x and p.z >= z and p.z <= z+10:
      gp.ChangeDutyCycle((x-p.x)*10)
      bp.ChangeDutyCycle((p.z-z)*10)
    if GPIO.input(k1) == False and pwm<100:
      pwm += 10
      rp.ChangeDutyCycle(pwm)
      time.sleep(0.1)
    if GPIO.input(k2) == False and pwm>0:
      pwm -= 10
      rp.ChangeDutyCycle(pwm)
      time.sleep(0.1)

except KeyboardInterrupt:
  GPIO.cleanup()
