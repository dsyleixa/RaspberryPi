#!/usr/bin/python
import mcpi.minecraft as minecraft
import pyautogui
import RPi.GPIO as GPIO

mc = minecraft.Minecraft.create()
kw = 16
ka = 21
ks = 12
kd = 7

GPIO.setmode(GPIO.BCM)
GPIO.setup(kw, GPIO.IN)
GPIO.setup(ka, GPIO.IN)
GPIO.setup(ks, GPIO.IN)
GPIO.setup(kd, GPIO.IN)

try:
  while True:
    if GPIO.input(kw) == False:
      pyautogui.keyDown('w')
    else:
      pyautogui.keyUp('w')
    if GPIO.input(ka) == False:
      pyautogui.keyDown('a')
    else:
      pyautogui.keyUp('a')
    if GPIO.input(ks) == False:
      pyautogui.keyDown('s')
    else:
      pyautogui.keyUp('s')
    if GPIO.input(kd) == False:
      pyautogui.keyDown('d')
    else:
      pyautogui.keyUp('d')
    
except KeyboardInterrupt:
  GPIO.cleanup()
