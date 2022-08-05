import RPi.GPIO as GPIO
from picamera import PiCamera
import time
import os


# Dictionary with GPIO ID for specific light angle

class Stand:
    """LightsGPIO = {
       0: 21,
        45: 21,
        90: 21,
        135: 21,
        180: 21,
        225: 21,
        270: 21,
        315: 21
    }"""
    LightsGPIO = {
        0: 5,
        45: 6,
        90: 12,
        135: 13,
        180: 16,
        225: 19,
        270: 20,
        315: 21
    }
    shot_delay = 0.2

    def __init__(self, input_filename_prefix, 
            environment_filename, resolution):
        self.camera = PiCamera()
        self.camera.resolution = resolution
        self.camera.rotation = 180

        GPIO.setmode(GPIO.BCM)
        #GPIO.setwarnings(False)
        for GPIOID in Stand.LightsGPIO.values():
            GPIO.setup(GPIOID, GPIO.OUT, initial=0)

        self.input_filename_prefix = input_filename_prefix
        self.environment_filename = environment_filename
        self.resolution = resolution

        self.__IntroTest()

    def __del__(self):
        self.camera.close()
        GPIO.cleanup()


    def __TurnAllLightsOn(self):
        for lightGPIO in self.LightsGPIO.values():
            GPIO.output(lightGPIO, GPIO.HIGH)

    def __TurnAllLightsOff(self):
        for lightGPIO in self.LightsGPIO.values():
            GPIO.output(lightGPIO, GPIO.LOW)

    def __IntroTest(self):
        intro_lights_change_delay = 0.05
        self.__TurnAllLightsOff()
        for _ in range(0,2):
            for lightGPIO in self.LightsGPIO.values():
                GPIO.output(lightGPIO, GPIO.HIGH)
                time.sleep(intro_lights_change_delay)
                GPIO.output(lightGPIO, GPIO.LOW)

    def __makeSingleShot(self, angle, path):
        GPIO.output(Stand.LightsGPIO[angle], GPIO.HIGH)
        time.sleep(self.shot_delay)
        self.camera.capture(path + "/" + 
            self.input_filename_prefix + str(angle) + ".bmp")
        time.sleep(self.shot_delay)
        GPIO.output(Stand.LightsGPIO[angle], GPIO.LOW)

    def __makeEnviromentLightShot(self, path):
        self.camera.capture(path + "/" + 
            self.input_filename_prefix + self.environment_filename + ".bmp")


    def gatherAllAssets(self, path):
        self.__makeEnviromentLightShot(path)
        for angle in self.LightsGPIO.keys():
            self.__makeSingleShot(angle, path)

    def check_camera_to_path(self, path):
        self.__TurnAllLightsOn()
        time.sleep(self.shot_delay)
        self.camera.capture(path)
        time.sleep(self.shot_delay)
        self.__TurnAllLightsOff()

