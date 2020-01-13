import time
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

b18 = 18
GPIO.setup(b18, GPIO.IN, GPIO.PUD_UP)
b16 = 16
GPIO.setup(b16, GPIO.IN, GPIO.PUD_UP)
b20 = 20
GPIO.setup(b20, GPIO.IN, GPIO.PUD_UP)
b21 = 21
GPIO.setup(b21, GPIO.IN, GPIO.PUD_UP)

while True:
  button_state = GPIO.input(b18)
  if button_state == GPIO.HIGH:
    print ("HIGH (b18 up)")
  else:
    print ("LOW (b18 dn)")

  button_state = GPIO.input(b16)
  if button_state == GPIO.HIGH:
    print ("HIGH (b16 up)")
  else:
    print ("LOW (b16 dn)")

  button_state = GPIO.input(b20)
  if button_state == GPIO.HIGH:
    print ("HIGH (b20 up)")
  else:
    print ("LOW (b20 dn)")

  button_state = GPIO.input(b21)
  if button_state == GPIO.HIGH:
    print ("HIGH (b21 up)")
  else:
    print ("LOW (b21 dn)")

  print ("") 




    
  time.sleep(0.5)
