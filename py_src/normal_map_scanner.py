import os
from PIL import Image
import numpy as np

from kivy.app import App
from kivy.core.window import Window
from kivy.uix.gridlayout import GridLayout
from kivy.uix.label import Label
from kivy.uix.image import Image as uiImage
from kivy.uix.button import Button
from kivy.uix.textinput import TextInput
from kivy.uix.screenmanager import ScreenManager, Screen
from kivy.logger import Logger
import logging

from normal_map.normal_map import NormalMap

try:
    from stand_driver import Stand
except:
    print("Running in no raspbery mode!")


# Properties
sourcefiles = dict()
input_filename_prefix = "input_"
assets_directory = "./input_assets/"
environment_filename = "all_off"
output_file = "normalmap.bmp"
resolution = (300, 300) #px
object_size = (20, 20) #cm
lamp_0_position = (40, 0, 20) #cm
verbosity = 1 # Only status
tmp_dir = "./tmp/"

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
                          environment_light=environment_light)

    normalmap.calculateNormalMap(verbosity=verbosity, log=True, 
        progressbar=True)

    if verbosity >= 1:
        normalmap.normalmap.show()
        print('Saving')
        
    normalmap.normalmap.save(output_file)

# Stand object
g_stand = Stand(assets_directory=assets_directory,
        input_filename_prefix=input_filename_prefix,
        environment_filename=environment_filename,
        resolution = resolution)

def main():
    # Ensure that assets directory exist
    if not os.path.exists(assets_directory):
        os.makedirs(assets_directory, exist_ok=True)
    g_stand.gatherAllAssets()
    calculateNormalMap()

# -----------------------------------------------------------------------------
#                                    GUI
# -----------------------------------------------------------------------------

Logger.setLevel(logging.TRACE)
#Window.fullscreen = True
Window.size = (960, 540)

class MenuScreen(Screen):
    pass

class FullProcessPart1Screen(Screen):
    pass

class GatherScreen(Screen):
    def check_camera(self):
        if not os.path.exists(tmp_dir):
            os.makedirs(tmp_dir, exist_ok=True)
        g_stand.check_camera_to_path(tmp_dir + "tmp.png")
        self.ids.check_image.reload()

class CalculateScreen(Screen):
    pass

class CalibrateScreen(Screen):
    pass

class NormalMapScannerApp(App):

    def build(self):
        # Create the screen manager
        sm = ScreenManager()
        sm.add_widget(MenuScreen(name='menu_screen'))
        sm.add_widget(FullProcessPart1Screen(name='full_process_part_1_screen'))
        sm.add_widget(GatherScreen(name='gather_screen'))
        sm.add_widget(CalculateScreen(name='calculate_screen'))
        sm.add_widget(CalibrateScreen(name='calibrate_screen'))

        return sm

if __name__ == '__main__':
    NormalMapScannerApp().run()