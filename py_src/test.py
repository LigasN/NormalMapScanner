# Uncomment these lines to see all the messages
# from kivy.logger import Logger
# import logging
# Logger.setLevel(logging.TRACE)

from kivy.app import App
from kivy.lang import Builder
from kivy.uix.boxlayout import BoxLayout
from camera4kivy import Preview

import time
Builder.load_string('''
<CameraClick>:
    orientation: 'vertical'
    Preview:
        id: camera
	    aspect_ratio: '16:9'
    ToggleButton:
        text: 'Play'
        on_press: root.start_preview()
        size_hint_y: None
        height: '48dp'
    Button:
        text: 'Capture'
        size_hint_y: None
        height: '48dp'
        on_press: root.capture()
''')


class CameraClick(BoxLayout):
    def build(self):
        self.preview = Preview(aspect_ratio = '16:9')
        self.preview.connect_camera(enable_analyze_pixels = True)

    def start_preview(self):
        camera = self.ids['camera']
        camera.start_preview()

    def capture(self):
        '''
        Function to capture the images and give them the names
        according to their captured time and date.
        '''
        camera = self.ids['camera']
        timestr = time.strftime("%Y%m%d_%H%M%S")
        camera.export_to_png("IMG_{}.png".format(timestr))
        print("Captured")


class TestCamera(App):

    def build(self):
        return CameraClick()


TestCamera().run()