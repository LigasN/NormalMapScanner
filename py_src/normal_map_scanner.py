from multiprocessing.reduction import steal_handle
import os
from PIL import Image
import numpy as np

from normal_map import NormalMap

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
LightsGPIO = {
    0: 0,
    45: 1,
    90: 2,
    135: 3,
    180: 4,
    225: 5,
    270: 6,
    315: 7
}

def makeSingleShot(angle):


def gatherAllAssets():
    # Ensure that assets directory exist
    assets_directory = os.path.dirname(os.path.abspath(assets_directory))
    if not os.path.exists(assets_directory):
        os.makedirs(assets_directory, exist_ok=True)

    for angle in LightsGPIO.keys():
        makeSingleShot(angle)
    return 0 

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
