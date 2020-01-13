#!/usr/bin/python
import mcpi.minecraft as minecraft
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
LED = 25

GPIO.setmode(GPIO.BCM)
GPIO.setup(LED, GPIO.OUT, initial=False)

pwm = 0
l = GPIO.PWM(LED, 50)
l.start(pwm)

try:
  while True:
    p = mc.player.getTilePos()
    if p.z>=5 and p.z<=15:
      pwm = 10*(15-p.z)
    l.ChangeDutyCycle(pwm)
    time.sleep(0.1)

except KeyboardInterrupt:
  l.stop()
  GPIO.cleanup()
