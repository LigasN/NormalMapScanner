# Uncomment these lines to see all the messages
# from kivy.logger import Logger
# import logging
# Logger.setLevel(logging.TRACE)

from kivy.lang import Builder
from kivy.app import App
from kivy.core.window import Window
from kivy.uix.gridlayout import GridLayout
from kivy.uix.label import Label
from kivy.uix.image import Image as uiImage
from kivy.uix.button import Button
from kivy.uix.textinput import TextInput
from kivy.uix.screenmanager import ScreenManager, Screen, NoTransition
from kivy.uix.widget import Widget
from kivy.logger import Logger
import logging

import time


Builder.load_string('''
<MyLayout>:
    FloatLayout:
        Image:
            id: normal_map_image
            size_hint_y: 0.7
            allow_stretch: True
            keep_ratio: True
            pos_hint: {'center_x': 0.5, 'center_y': 0.45}
            source: tmp_dir + 'normalmap.bmp'
        BoxLayout:
            size_hint: (1, 0.5)
            pos_hint: {'center_x': 0.5, 'center_y': 0.1}
            orientation: "horizontal"
            Label:
                text: 'Assets path: '
                font_size: self.width * 0.15
                halign: 'left'
                valign: 'middle'
            Spinner:
                id: spinner_id
                font_size: self.width * 0.05
                text: "Click to choose assets path"
                values: ["sth1", "sth2", "sth4"]
                on_text: root.spinner_clicked(spinner_id.text)
''')


class MyLayout(Screen):
    def spinner_clicked(self, value):
        self.ids.click_label.text = value

class NormalMapScannerApp(App):

    def build(self):
        # Create the screen manager
        sm = ScreenManager()
        sm.transition = NoTransition()
        sm.add_widget(MyLayout(name='my_layout'))
        
        return sm


if __name__ == '__main__':
    NormalMapScannerApp().run()