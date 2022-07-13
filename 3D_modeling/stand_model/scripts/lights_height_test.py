import bpy
from bpy import context
import builtins as __builtin__
import os
import numpy as np
import math

output_dir = 'C:/Users/norbe/Repozytoria/NormalMapScanner/3D_modeling/scripts/tmp'
output_folder_pattern = 'lights_height_%d'
output_file_prefix = "input_"
output_file_format = 'bmp'

bpy.context.scene.render.image_settings.color_mode = 'RGB'
bpy.context.scene.render.image_settings.color_depth = '8'
bpy.context.scene.render.image_settings.file_format = 'BMP'

start_height = 0.05 # starting height in meters
step_height = 0.05 # step in meters
stop_height = 0.50 # stop height in meters

lights = bpy.data.collections['Lights'].all_objects.values()
original_height = lights[0].location[2]
light_distance = lights[0].location[0] # light distance from center of the object
original_engine = 'CYCLES';


"""Console print() function."""
def console_print(*args, **kwargs):
    for a in context.screen.areas:
        if a.type == 'CONSOLE':
            c = {}
            c['area'] = a
            c['space_data'] = a.spaces.active
            c['region'] = a.regions[-1]
            c['window'] = context.window
            c['screen'] = context.screen
            s = " ".join([str(arg) for arg in args])
            for line in s.split("\n"):
                bpy.ops.console.scrollback_append(c, text=line)
                
def print(*args, **kwargs):
    console_print(*args, **kwargs) # to py consoles
    __builtin__.print(*args, **kwargs) # to system console
"""-----------------------------------------------------------"""
    
def change_lights_height(height):
    angle = math.atan(light_distance/height)# y rotation of light source to beam on the center of the object
    for light in lights:
        light.location[2] = height
        light.rotation_euler[1] = -angle
    print("Lights height is set to " + str(height))
    print("Lights angle is set to " + str(angle))
        
def turn_lights_off():
    for light in lights:
        light.hide_render = True

def make_images(dir):
    print("Rendering images to directory: " + dir)
    for light in lights:
        filename = str(output_file_prefix + light.name + '.' + output_file_format).lower()
        print("Rendering: " + filename)
        light.hide_render = False
        bpy.context.scene.render.filepath = os.path.join(dir, filename)
        bpy.ops.render.render(write_still = True)
        print("Image saved: " + bpy.context.scene.render.filepath)
        light.hide_render = True
    
def main():
    original_engine = bpy.context.scene.render.engine
    bpy.context.scene.render.engine = 'CYCLES'
    print("Engine set to " + bpy.context.scene.render.engine)
    
    turn_lights_off()
    print("Lights turned off")
    
    for height in np.arange(start_height, stop_height, step_height):
        change_lights_height(height)
        dir = os.path.join(output_dir, str(output_folder_pattern % (height * 100)) + 'cm')
        make_images(dir)
        
    change_lights_height(original_height)
    bpy.context.scene.render.engine = original_engine
    
    
    
if __name__ == "__main__":
    main()