#https://forbot.pl/blog/kurs-raspberry-pi-podstawy-pythona-gpio-id26099

import RPi.GPIO as GPIO
#from time import *


turnOn = True
run = True
GPIO.setmode(GPIO.BCM)
#GPIO.setwarnings(False)
GPIOId = 21
GPIO.setup(GPIOId, GPIO.OUT, initial=0)


def processInput():
    tmp = input("Turn off - 0, Turn on - 1, Quit - q\nInput: ")
    global turnOn
    global run
    if tmp == '0':
         turnOn = False
    elif tmp == '1':
        turnOn = True
    elif tmp == 'q':
        run = False
        
    print("turnOn: " + str(turnOn) + " run: " + str(run))
    
def main():
    while(run):
        processInput()
        GPIOState = GPIO.input(GPIOId)
        print("GPIOState: " + str(GPIOState))
        if turnOn == True and GPIOState == GPIO.LOW:
            print("Turning on")
            GPIO.output(GPIOId, GPIO.HIGH)
            GPIOState = GPIO.input(GPIOId)
            print("GPIOState after change: " + str(GPIOState))
        elif turnOn == False and GPIOState == GPIO.HIGH:
            print("Turning off")
            GPIO.output(GPIOId, GPIO.LOW)
            GPIOState = GPIO.input(GPIOId)
            print("GPIOState after change: " + str(GPIOState))
            
    GPIO.cleanup()
    

if __name__ == "__main__":
    main()
