if __name__ == "__main__":

    from PIL import Image
    import numpy as np
    import os

    from normal_map import NormalMap

    def load_image(filename):
        try:
            return Image.open(
                filename)
        except FileNotFoundError:
            print('File not found: ' + filename)


    # Properties
    verbosity = 1 # Only status

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
        # test assets location
        assets_path = 'test_input/4.blender_1200x1200px/'  
        image_size = np.array([1200, 1200])  # px
        object_size = np.array([20, 20])  # cm
        lamp_0_position = np.array([40, 0, 15])  # cm
    elif(test == "Photos 2200x2200 px"):
        # test assets location
        assets_path = 'test_input/5.photos_2200x2200px/'  
        image_size = np.array([2200, 2200])  # px test image size
        object_size = np.array([20, 20])  # cm virtual test object size
        lamp_0_position = np.array([40, 0, 22])  # cm

    input_filename_prefix = "input_"
    output_file = "./tmp/normalmap.bmp"
    environment_filename = "all_off"

    output_path = os.path.dirname(os.path.abspath(output_file))
    if not os.path.exists(output_path):
        os.makedirs(output_path, exist_ok=True)

    sourcefiles = dict()
    for angle in NormalMap.angles:
        sourcefiles[angle] = load_image(
            assets_path + input_filename_prefix + str(angle) +
            '.bmp')

    environment_light = load_image(
        assets_path + input_filename_prefix +
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