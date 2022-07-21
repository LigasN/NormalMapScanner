
from normal_map.normal_map_calculator import calculate_normal_map
import numpy as np


calculate_normal_map

# Properties
# debugging = 1 # True
debugging = 0  # False


# test = "All black 3x3 px"
# test = "All white 3x3 px"
# test = "Simple test 3x3 px"
# test = "Blender 1200x1200 px"
test = "Photos 2200x2200 px"

if(test == "All black 3x3 px"):
    assets_path = 'test_input/1.all_black_3x3px/'  # test assets location
    image_size = np.array([3, 3])  # px test image size
    object_size = np.array([3, 3])  # cm virtual test object size
    lamp_0_position = np.array([40, 0, 15])  # cm
elif(test == "All white 3x3 px"):
    assets_path = 'test_input/2.all_white_3x3px/'  # test assets location
    image_size = np.array([3, 3])  # px test image size
    object_size = np.array([3, 3])  # cm virtual test object size
    lamp_0_position = np.array([0, 0, 15])  # cm
elif(test == "Simple test 3x3 px"):
    assets_path = 'test_input/3.simple_test_3x3px/'  # test assets location
    image_size = np.array([4, 4])  # px test image size
    object_size = np.array([4, 4])  # cm virtual test object size
    lamp_0_position = np.array([40, 0, 15])  # cm
elif(test == "Blender 1200x1200 px"):
    assets_path = 'test_input/4.blender_1200x1200px/'  # test assets location
    image_size = np.array([1200, 1200])  # px
    object_size = np.array([20, 20])  # cm
    lamp_0_position = np.array([40, 0, 15])  # cm
elif(test == "Photos 2200x2200 px"):
    assets_path = 'test_input/5.photos_2200x2200px/'  # test assets location
    image_size = np.array([2200, 2200])  # px test image size
    object_size = np.array([20, 20])  # cm virtual test object size
    lamp_0_position = np.array([40, 0, 22])  # cm

input_filename_prefix = "input_"
output_file = "normalmap.bmp"

name = input_filename_prefix + str(angles[a]) + '.bmp'

# Load image
im = load_image(name)