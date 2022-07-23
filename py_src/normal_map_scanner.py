import os
from PIL import Image
import numpy as np

import RPi.GPIO as GPIO
from picamera import PiCamera
import time

from normal_map.normal_map import NormalMap

# Properties
sourcefiles = dict()
input_filename_prefix = "input_"
assets_directory = "./input_assets/"
environment_filename = "all_off"
output_file = "normalmap.bmp"
verbosity = 1 # Only status

#def load_image(filename):
#    try:
#        return Image.open(
#            filename)
#    except FileNotFoundError:
#        print('File not found: ' + filename)

# Dictionary with GPIO ID for specific light angle
"""LightsGPIO = {
    0: 5,
    45: 6,
    90: 12,
    135: 13,
    180: 16,
    225: 19,
    270: 20,
    315: 21
}"""
LightsGPIO = {
    0: 21,
    45: 21,
    90: 21,
    135: 21,
    180: 21,
    225: 21,
    270: 21,
    315: 21
}

camera = PiCamera()
GPIO.setmode(GPIO.BCM)
#GPIO.setwarnings(False)
for GPIOID in LightsGPIO.values():
    print("Setting GPIOID %d as output", GPIOID)
    GPIO.setup(GPIOID, GPIO.OUT, initial=0)


def makeSingleShot(angle):
    GPIO.output(LightsGPIO[angle], GPIO.HIGH)
    time.sleep(1)
    camera.capture(assets_directory + "/input_" + str(angle) + ".bmp")
    GPIO.output(LightsGPIO[angle], GPIO.LOW)
    time.sleep(1)


def gatherAllAssets():
    # Ensure that assets directory exist
    if not os.path.exists(assets_directory):
        os.makedirs(assets_directory, exist_ok=True)

    for angle in LightsGPIO.keys():
        makeSingleShot(angle)

    camera.close()
    GPIO.cleanup()


"""def calculateNormalMap():
    for angle in NormalMap.angles:
        sourcefiles[angle] = load_image(
            assets_directory + input_filename_prefix + str(angle) +
            '.bmp')

    environment_light = load_image(
        assets_directory + input_filename_prefix +
        environment_filename + '.bmp')

    normalmap = NormalMap(source_files=sourcefiles,
                          object_size=object_size,
                          lamp_0_position=lamp_0_position,
                          environment_light=environment_light)

    normalmap.calculateNormalMap(verbosity=verbosity, log=True, 
        progressbar=True)

    if verbosity >= 1:
        normalmap.normalmap.show()
        print('Saving')
        
    normalmap.normalmap.save(output_file)
"""


def main():
    gatherAllAssets()
    #calculateNormalMap()


if __name__ == "__main__":
    main()
