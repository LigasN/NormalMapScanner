import RPi.GPIO as GPIO
from picamera import PiCamera
import time
import os


# Dictionary with GPIO ID for specific light angle

class Stand:
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

    def __makeSingleShot(self, angle, output_filepath):
        GPIO.output(Stand.LightsGPIO[angle], GPIO.HIGH)
        time.sleep(self.shot_delay)
        self.camera.capture(output_filepath)
        time.sleep(self.shot_delay)
        GPIO.output(Stand.LightsGPIO[angle], GPIO.LOW)

    def __makeEnviromentLightShot(self, path):
        self.camera.capture(path + "/" + 
            self.input_filename_prefix + self.environment_filename + ".bmp")


    def setResolution(self, resolution):
        self.camera.resolution = resolution

    def getResolution(self):
        return self.camera.resolution


    def gatherAllAssets(self, save_path, preview_callback = None):
        self.__makeEnviromentLightShot(save_path)
        output_filepath = None 
        for angle in self.LightsGPIO.keys():
            output_filepath = os.path.join(os.path.abspath(save_path),
                self.input_filename_prefix + str(angle) + ".bmp")
            self.__makeSingleShot(angle, output_filepath)
        if preview_callback:
            preview_callback(output_filepath)


    def checkCameraToPath(self, path):
        self.__TurnAllLightsOn()
        time.sleep(self.shot_delay)
        self.camera.capture(path)
        time.sleep(self.shot_delay)
        self.__TurnAllLightsOff()

