import os
import sys
import numpy as np
import time
from PIL import Image, ImageChops
import datetime

# Properties
# test = "Real input"
# test = "All black input"
# test = "All white input"
test = "Simple test"
if(test == "Real input"):
    image_size = np.array([1200, 1200])  # px
    object_size = np.array([20, 20])  # cm
    assets_path = '../assets/'
    lamp_0_position = np.array([40, 0, 15])  # cm
elif(test == "All black input"):
    image_size = np.array([3, 3])  # px test image size
    object_size = np.array([3, 3])  # cm virtual test object size
    assets_path = '../assets/test_assets/all_black/'  # test assets location
    lamp_0_position = np.array([40, 0, 15])  # cm
elif(test == "All white input"):
    image_size = np.array([3, 3])  # px test image size
    object_size = np.array([3, 3])  # cm virtual test object size
    assets_path = '../assets/test_assets/all_white/'  # test assets location
    lamp_0_position = np.array([0, 0, 15])  # cm
elif(test == "Simple test"):
    image_size = np.array([4, 4])  # px test image size
    object_size = np.array([4, 4])  # cm virtual test object size
    assets_path = '../assets/test_assets/simple_test/'  # test assets location
    lamp_0_position = np.array([40, 0, 15])  # cm


R, G, B = 0, 1, 2
X, Y, Z = 0, 1, 2
angles = np.array(range(0, 360, 45))
angles_rad = np.array((angles * np.pi) / 180, float)


def sRGB2Linear(im):
    # Convert to numpy array of float
    arr = np.array(im, dtype=np.float32) / 255.0
    # Convert sRGB -> linear
    arr = np.where(arr <= 0.04045, arr/12.92, ((arr+0.055)/1.055)**2.4)
    # Convert to 8-bit
    arrOut = np.uint8(np.rint(arr * 255.0))
    # Convert back to PIL
    return Image.fromarray(arrOut)


def magnitude(v):
    return np.sqrt(np.sum(v**2))


def normalize(v):
    mag = magnitude(v)
    if(mag == 0.0):  # prevents dividing by zero
        # NINF value suits calculations with images
        return np.full(np.shape(v), -np.NINF)
    return v / magnitude(v)


def load_image(filename):
    try:
        return Image.open(
            assets_path + filename)
    except FileNotFoundError:
        print('File not found: ' + assets_path + filename)


def main():
    # Creating tmp directory for future purposes
    if not os.path.exists("./tmp/"):
        os.makedirs("./tmp/")
    
    # Logging initialization
    sys.stdout = open('./tmp/log.txt', 'w')
    a = datetime.datetime.now()
    print(str(datetime.datetime.now()) + '\n')

    # single pixel stores sum of RGB colors
    input = np.zeros((8, image_size[1], image_size[0]), float)
    output = np.zeros((image_size[1], image_size[0], 3), float)

    print('Loading of the environment light image.')
    environment_light = load_image(
        'input_all_off.bmp')

    for a in range(0, angles.size):
        print('Loading of the input data array with angle: %d' %
              angles[a])
        start_time = time.time()
        name = 'input_' + str(angles[a]) + '.bmp'

        # Load image
        im = load_image(name)

        # Subtract environment light
        im = ImageChops.subtract(
            im, environment_light)

        # Sum colors
        input[a] = (np.array(im.getdata(R), float) +
                    np.array(im.getdata(G), float) +
                    np.array(im.getdata(B), float)).reshape(
            image_size[1], image_size[0])
        print("Loaded and converted in %d ms" %
              ((time.time() - start_time) * 1000))
    print(input)
    print('Calcultion of the positions of the lamps')
    start_time = time.time()
    lamp_pos = np.empty((8, 3))
    lamp_pos[0] = lamp_0_position
    lamp_distance = lamp_0_position[0]
    for angle_idx in range(angles_rad.size):
        # Calculate every lamp position (z position is not changing)
        if angles_rad[angle_idx] != 0:
            lamp_pos[angle_idx][0] = np.cos(
                angles_rad[angle_idx]) * lamp_distance
            lamp_pos[angle_idx][1] = np.sin(
                angles_rad[angle_idx]) * lamp_distance
            lamp_pos[angle_idx][2] = lamp_0_position[2]
    print("Position of lamps calculated in %d ms" %
          ((time.time() - start_time) * 1000))
    print(lamp_pos)
    print('Calculation of the normalmap vectors')
    start_time = time.time()
    pixel_size = object_size / image_size
    pixel_idx = np.zeros(2)
    for y, x in np.ndindex(image_size[1], image_size[0]):
        # (pixel position for an object aligned with its upper left corner
        # to (0,0)) and -/+ to center results back)
        pixel_idx = np.array((x, y))
        pixel_pos = (((pixel_idx * pixel_size) +
                     (np.array((pixel_size[0], (-pixel_size[1]))) / 2)) -
                     (np.array((object_size[0], (-object_size[1]))) / 2))
        pixel_pos = np.append(pixel_pos, 0.)  # Z axis

        N_vector = np.zeros(3, float)
        for angle_idx in range(angles_rad.size):
            # Vector pointing to the light source
            L_vector = lamp_pos[angle_idx] - pixel_pos
            # L_vector is pointing to the lamp. Input image for normal vector
            # equal to this should be white (255,255,255). For opposite
            # vector color would be black (0,0,0).
            weight = input[angle_idx][y][x] / 765.0
            N_vector += L_vector * weight
        output[y][x] = normalize(N_vector)
    print("Normalmap vectors calculated in %d ms" %
          ((time.time() - start_time) * 1000))

    print('Conversion back to PIL')
    start_time = time.time()
    image_data = np.array((((output + 1.0) / 2.0) * 255.0), np.uint8)
    normalmap = Image.fromarray(image_data, mode='RGB')
    print("Convertion back to PIL done in %d ms" %
          ((time.time() - start_time) * 1000))

    # Create output image object
    normalmap.show()
    print('Saving')
    normalmap.save("./tmp/normalmap.bmp")

    # Printing debug stuff
    print("output")
    print(output)

    # Close logging
    sys.stdout.close()


if __name__ == "__main__":
    main()
