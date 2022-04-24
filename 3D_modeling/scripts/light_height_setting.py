import bpy
from bpy import context
import builtins as __builtin__

height = 0.15 # in meters

lights = bpy.data.collections['Lights'].all_objects.values()

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

for light in lights:
    light.location[2] = height
    print(light.name + " height is set to " + str(height))