#!/usr/bin/python
import mcpi.minecraft as minecraft
import RPi.GPIO as GPIO
import time

mc = minecraft.Minecraft.create()
rot    = 0
gelb   = 1
gruen  = 2
Ampel  = [18,23,25]

GPIO.setmode(GPIO.BCM)
GPIO.setup(Ampel[rot], GPIO.OUT, initial=True)
GPIO.setup(Ampel[gelb], GPIO.OUT, initial=False)
GPIO.setup(Ampel[gruen], GPIO.OUT, initial=False)

try:
    while True:
        p = mc.player.getTilePos()
        mat = mc.getBlock(p.x, p.y-1, p.z)
        if mat==98:        
            GPIO.output(Ampel[gelb],True)
            time.sleep(0.6)
            GPIO.output(Ampel[rot],False)
            GPIO.output(Ampel[gelb],False)
            GPIO.output(Ampel[gruen],True)
            time.sleep(2)
            GPIO.output(Ampel[gruen],False)
            GPIO.output(Ampel[gelb],True)
            time.sleep(0.6)
            GPIO.output(Ampel[gelb],False)
            GPIO.output(Ampel[rot],True)
            time.sleep(2)

except KeyboardInterrupt:
    GPIO.cleanup()
        
    

        
