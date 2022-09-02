import os
from PIL import Image
import numpy as np

from normal_map.normal_map import NormalMap
from stand_driver import Stand

from datetime import datetime

# Properties
sourcefiles = dict()
input_filename_prefix = "input_"
assets_directory = "./input_assets/"
environment_filename = "all_off"
output_file = assets_directory + "normalmap.bmp"
resolution = (800, 800) #px
object_size = (20, 20) #cm
lamp_0_position = (40, 0, 20) #cm
verbosity = 1 # Only status

if not os.path.exists("./tmp/"):
    os.makedirs("./tmp/")

logfile = open('./tmp/log.txt', 'w')

def log( message):
    print(message)
    if logfile: 
        logfile.write(message)

def log_progress(percentage):
    log("Calculation progress (" + str(datetime.now().time()) + "): " + "{:.2f}".format(percentage) + '%')

log(str(datetime.now()) + '\n')

def load_image(filename):
    try:
        return Image.open(filename)
    except FileNotFoundError:
        print('File not found: ' + filename)


def calculateNormalMap():
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
                          environment_light=environment_light,
                          )

    normalmap.calculateNormalMap(verbosity=verbosity, log=False, 
        progressbar=True, set_progress_bar_value_function = log_progress)

    if verbosity >= 1:
        normalmap.normalmap.show()
        print('Saving')
        
    normalmap.normalmap.save(output_file)


def main():
    stand = Stand(assets_directory=assets_directory,
        input_filename_prefix=input_filename_prefix,
        environment_filename=environment_filename,
        resolution = resolution
        )
    # Ensure that assets directory exist
    if not os.path.exists(assets_directory):
        os.makedirs(assets_directory, exist_ok=True)
    
    i = 0
    while(True):

        if not os.path.exists(assets_directory):
            os.makedirs(assets_directory, exist_ok=True)

        log("gathering assets[" + str(i) + "]: " + str(datetime.now().time()))
        stand.gatherAllAssets()

        log("calculating map[" + str(i) + "]: " + str(datetime.now().time()))
        calculateNormalMap()

        try:
            os.rmdir(assets_directory)
        except OSError as e:
            print("Error: %s : %s" % (assets_directory, e.strerror))
        
        i = i + 1
        


if __name__ == "__main__":
    main()