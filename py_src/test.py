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
from kivy.uix.screenmanager import ScreenManager, Screen
from kivy.uix.widget import Widget
from kivy.logger import Logger
import logging

import time
Builder.load_string('''
<MyLayout>:
    BoxLayout:
        orientation: "vertical"
        size: root.width, root.height

        Label:
            id: click_label
            text: "Pick"
            font_size: 32
        
        Spinner:
            id: spinner_id
            text: "sth fav"
            values: ["sth1", "sth2", "sth4"]
            on_text: root.spinner_clicked(spinner_id.text)
''')


class MyLayout(Widget):
    def spinner_clicked(self, value):
        self.ids.click_label.text = value

class AwesomeApp(App):
    def build(self):
        return MyLayout()


if __name__ == '__main__':
    AwesomeApp().run()