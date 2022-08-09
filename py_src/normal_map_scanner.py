import os
import sys
from threading import Thread
from PIL import Image
import numpy as np
from datetime import datetime
import glob

from kivy.app import App
from kivy.core.window import Window
from kivy.uix.widget import Widget
from kivy.uix.gridlayout import GridLayout
from kivy.uix.floatlayout import FloatLayout
from kivy.uix.label import Label
from kivy.uix.image import Image as uiImage
from kivy.uix.button import Button
from kivy.uix.textinput import TextInput
from kivy.uix.screenmanager import ScreenManager, Screen, NoTransition
from kivy.properties import ObjectProperty, StringProperty
from kivy.uix.popup import Popup
from kivy.utils import platform
from kivy.logger import Logger
from kivy.clock import Clock
import logging

from normal_map.normal_map import NormalMap

try:
    from stand_driver import Stand
except:
    print("Running in no raspbery mode!")


# Properties
sourcefiles = dict()
input_filename_prefix = "input_"
assets_parent_directory = "input_assets/"
tmp_dir = "tmp/"
workspace_path = "./"
environment_filename = "all_off"
output_file = "normalmap.bmp"
resolution = (800, 800) #px
object_size = (20, 20) #cm
lamp_0_position = (40, 0, 20) #cm
verbosity = 1 # Only status

def load_image(filename):
    try:
        return Image.open(filename)
    except FileNotFoundError:
        print('File not found: ' + filename)


def calculateNormalMap(session_abs_path, set_progress_bar_value_function, 
        is_asked_to_exit, is_done_callback):
    tmp_path = os.path.join(session_abs_path, tmp_dir)
    if not os.path.exists(tmp_path):
        os.makedirs(tmp_path, exist_ok=True)

    assets_path = os.path.join(session_abs_path, assets_parent_directory)
    load_angle_assets_path = os.path.join(os.path.abspath(assets_path))
    for angle in NormalMap.angles:
        filename = input_filename_prefix + str(angle) + '.bmp'
        filepath = os.path.join(load_angle_assets_path, filename)
        sourcefiles[angle] = load_image(filepath)

    environment_file_name = input_filename_prefix + environment_filename + '.bmp'
    environment_path = os.path.join(os.path.abspath(assets_path), environment_file_name)
    environment_light = load_image(environment_path)

    normalmap = NormalMap(source_files=sourcefiles,
                          object_size=object_size,
                          lamp_0_position=lamp_0_position,
                          environment_light=environment_light)

    normalmap.calculateNormalMap(verbosity=verbosity, log=True, 
        progressbar=True, set_progress_bar_value_function=set_progress_bar_value_function,
        is_asked_to_exit=is_asked_to_exit)

    if normalmap.normalmap:
        if verbosity >= 1:
            normalmap.normalmap.show()
            print('Saving')

        normalmap.normalmap.save(os.path.join(session_abs_path, output_file))
    is_done_callback()


# Stand object
try:
    g_stand = Stand(input_filename_prefix=input_filename_prefix,
        environment_filename=environment_filename,
        resolution = resolution)
except:
    pass

# -----------------------------------------------------------------------------
#                                    GUI
# -----------------------------------------------------------------------------

#Logger.setLevel(logging.TRACE)
#Window.fullscreen = True
Window.size = (960, 540)


class ChooseWorkspaceDialog(FloatLayout):
    change_workspace = ObjectProperty(None)
    cancel = ObjectProperty(None)

class MenuScreen(Screen):

    def dismiss_popup(self):
        self._popup.dismiss()

    def show_change_workspace_menu(self):
        content = ChooseWorkspaceDialog(change_workspace=self.change_workspace, cancel=self.dismiss_popup)
        self._popup = Popup(title="Change workspace", content=content,
                            size_hint=(0.9, 0.9))
        self._popup.open()

    def change_workspace(self, path):
        global workspace_path
        workspace = os.path.join(path)
        if os.path.isdir(workspace):
            workspace_path = workspace
            self.ids.workspace_id.refresh_workspace_path_label()
            self.dismiss_popup()

class FullProcessPart1Screen(Screen):
    pass

class GatherScreen(Screen):
    def on_pre_enter(self):
        self.ids.workspace_id.refresh_workspace_path_label()

    def check_camera(self):
        tmp_path = os.path.join(os.path.abspath(workspace_path), tmp_dir)
        if not os.path.exists(tmp_path):
            os.makedirs(tmp_path, exist_ok=True)
        check_filepath = tmp_path + "tmp.png"
        g_stand.check_camera_to_path(check_filepath)
        self.ids.source = check_filepath
        self.ids.check_image.reload()

    def capture_all_assets(self):
        session_dir = datetime.now().strftime('%Y-%m-%d_%H.%M.%S')
        save_dir = os.path.join(os.path.abspath(workspace_path), session_dir, assets_parent_directory)
        if not os.path.exists(save_dir):
            os.makedirs(save_dir, exist_ok=True)
        g_stand.gatherAllAssets(save_dir)

class CalculateScreen(Screen):
    session_path = os.path.abspath(workspace_path)
    gather_input_message_value = ["Please gather input images first"]
    assets_paths_list = []
    asked_to_exit = False

    def __init__(self,**kwargs):
        super(CalculateScreen, self).__init__(**kwargs)

    def __del__(self):
        self.asked_to_exit = True
        self.thread.join()

    def exit(self):
        self.asked_to_exit = True
        self.thread.join()
        self.manager.current = 'menu_screen'

    def is_asked_to_exit(self):
        if self.asked_to_exit:
            self.asked_to_exit = False
            return True
        return False

    def is_done(self):
        self.__refresh_normalmap()

    def __refresh_normalmap(self):
        self.ids.normal_map_image.source = os.path.join(os.path.abspath(self.session_path), output_file)
        if os.path.exists(self.ids.normal_map_image.source):
            self.ids.normal_map_image.reload()
        else:
            self.ids.normal_map_image.source = os.path.join("./assets/preview.jpg")
            self.ids.normal_map_image.reload()



    def on_pre_enter(self):
        self.ids.workspace_id.refresh_workspace_path_label()
        self.reload_current_assets_path()
        self.set_progress_bar_value(0)
        self.__refresh_normalmap()

    def reload_current_assets_path(self):
        self.assets_paths_list.clear()
        workspace_abs_path = os.path.abspath(workspace_path)
        if os.path.isdir(workspace_abs_path):
            for dir_name in os.listdir(workspace_abs_path):
                possible_assets_path = os.path.join(workspace_abs_path, dir_name, assets_parent_directory)
                possible_asset_file_path = os.path.join(possible_assets_path, "*.bmp")
                if os.path.isdir(possible_assets_path) and glob.glob(possible_asset_file_path):
                    self.assets_paths_list.append(dir_name)

        if self.assets_paths_list:
            self.ids.spinner_id.values = self.assets_paths_list
        else:
            self.ids.spinner_id.values = self.gather_input_message_value

    def spinner_clicked(self, value):
        self.session_path = os.path.join(os.path.abspath(workspace_path), value)
        self.__refresh_normalmap()

    def set_progress_bar_value(self, percentage_value):
        self.ids.progress_bar.value = percentage_value
        self.ids.progress_label.text = "{:.2f}".format(percentage_value) + '%'
        

    def calculate_normal_map(self):
        assets_path = os.path.join(self.session_path, assets_parent_directory, "*.bmp")
        if os.path.isdir(self.session_path) and glob.glob(assets_path):
            self.set_progress_bar_value(0)
            self.thread = Thread(target = calculateNormalMap, args = (self.session_path, self.set_progress_bar_value, self.is_asked_to_exit, self.is_done))
            self.thread.start()

class CalibrateScreen(Screen):
    pass

class WorkspaceWidget(FloatLayout):
    workspace_path_property = StringProperty(os.path.abspath(workspace_path))
    def refresh_workspace_path_label(self):
        self.ids.workspace_path_label.text = "Workspace path: " + os.path.abspath(workspace_path)

class NormalMapScannerApp(App):

    def build(self):
        # Create the screen manager
        sm = ScreenManager()
        sm.transition = NoTransition()
        sm.add_widget(MenuScreen(name='menu_screen'))
        sm.add_widget(FullProcessPart1Screen(name='full_process_part_1_screen'))
        sm.add_widget(GatherScreen(name='gather_screen'))
        sm.add_widget(CalculateScreen(name='calculate_screen'))
        sm.add_widget(CalibrateScreen(name='calibrate_screen'))

        return sm

if __name__ == '__main__':
    NormalMapScannerApp().run()
    sys.exit()
