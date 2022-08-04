import RPi.GPIO as GPIO
from picamera import PiCamera
import time


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
    def __init__(self, assets_directory, input_filename_prefix, 
            environment_filename, resolution):
        self.camera = PiCamera()
        self.camera.resolution = resolution

        GPIO.setmode(GPIO.BCM)
        #GPIO.setwarnings(False)
        for GPIOID in Stand.LightsGPIO.values():
            GPIO.setup(GPIOID, GPIO.OUT, initial=0)

        self.assets_directory = assets_directory
        self.input_filename_prefix = input_filename_prefix
        self.environment_filename = environment_filename
        self.resolution = resolution

    def __del__(self):
        self.camera.close()
        GPIO.cleanup()


    def __makeSingleShot(self, angle):
        GPIO.output(Stand.LightsGPIO[angle], GPIO.HIGH)
        time.sleep(0.2)
        self.camera.capture(self.assets_directory + "/" + 
            self.input_filename_prefix + str(angle) + ".bmp")
        GPIO.output(Stand.LightsGPIO[angle], GPIO.LOW)
        time.sleep(0.2)

    def __makeEnviromentLightShot(self):
        self.camera.capture(self.assets_directory + "/" + 
            self.input_filename_prefix + self.environment_filename + ".bmp")

    def gatherAllAssets(self):
        self.__makeEnviromentLightShot()
        for angle in self.LightsGPIO.keys():
            self.__makeSingleShot(angle)

