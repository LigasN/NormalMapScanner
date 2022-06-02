from picamera import PiCamera
import time

turnOn = False
run = True
capturing = False

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
    global run
    global turnOn
    global capturing
    camera = PiCamera()
    
    while(run):
        processInput()
        if turnOn == True and capturing == False:
            print("Turning on")
            capturing = True
            camera.start_preview()
            camera.start_recording('my_video.h264')
        if turnOn == False and capturing == True:
            print("Turning off")
            capturing = False
            camera.stop_recording()
            camera.stop_preview()
            
    camera.close()
    

if __name__ == "__main__":
    main()