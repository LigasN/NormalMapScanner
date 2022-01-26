import numpy as np
import math
import time
from tkinter import BitmapImage
from PIL import Image, ImageChops

# Properties
image_size = np.array([1200, 1200])  # px
object_size = np.array([20, 20])  # cm
assets_path = '../assets/'
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


def vec2_mag(vec2):
    return math.sqrt(vec2[0]**2 + vec2[1]**2)


def load_image(filename):
    try:
        return Image.open(
            assets_path + filename)
    except FileNotFoundError:
        print('File not found: ' + assets_path + filename)


def main():
    # single pixel stores sum of RGB colors
    input = np.zeros((8, image_size[1], image_size[0]), float)
    output = np.zeros((image_size[1], image_size[0], 3), float)

    print('Loading of the environment light image.')
    environment_light = load_image(
        '../assets/input_all_off.bmp')

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
        for y, x in np.ndindex(image_size[1], image_size[0]):
            input[a][y][x] = float(np.sum(im.getpixel(
                (x, y))))
        print("Loaded and converted in %d sec" % (time.time() - start_time))

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
    print("Position of lamps calculated in %d sec" %
          (time.time() - start_time))

    # TODO: calculation is done in the wrong way
    # It should be N = w1*v1 + w2*v2 + ...
    print('Calculation of the normalmap vectors')
    pixel_size = object_size / image_size
    pixel_idx = np.zeros(2)
    for y, x in np.ndindex(image_size[1], image_size[0]):
        # (pixel position for an object aligned with its upper left corner
        # to (0,0)) -/+ to center results back)
        pixel_idx = np.array((x, y))
        pixel_pos = ((pixel_idx * pixel_size) +
                     (np.array((pixel_size[0], (-pixel_size[1]))) / 2)) - (np.array((object_size[0], (-object_size[1]))) / 2)
        pixel_pos = np.append(pixel_pos, 0.)  # Z axis

        N_vectors = np.zeros((8, 3))
        for angle_idx in range(angles_rad.size):
            # Vector pointing to the light source
            L_vector = lamp_pos[angle_idx] - pixel_pos
            # L_vector is pointing to the lamp. Input image for normal vector
            # equal to this should be white (255,255,255). For opposite
            # vector color would be black (0,0,0).
            weight = input[angle_idx][y][x] / 765.
            NL_angle = (1 - weight) * np.pi
            # To move a vector without using quaterions it is moved on its axis created parallel to diagonal of X and Y values of the vector
            diagonal = vec2_mag(L_vector[:2])
            dz_mag = vec2_mag((diagonal, L_vector[2]))
            new_diagonal = np.cos(
                NL_angle) * dz_mag  # x axis
            N_vectors[angle_idx][2] = np.sin(
                NL_angle) * dz_mag  # y axis
            # Diagonal change ratio to get x and y back
            diag_ratio = new_diagonal / diagonal
            N_vectors[angle_idx][:2] = diag_ratio * L_vector[:2]
        # Averaging of the N vector results
        N_vector = np.average(N_vectors, axis=0)
        output[int(pixel_idx[1])][int(pixel_idx[0])][0] = N_vector[0]

    # Convert back to PIL
    # TODO: There need to be some sort of conversion to take normals into image space
    normalmap = Image.fromarray(output, mode='RGB')

    # Create output image object
    normalmap.show()
    print('Saving')
    normalmap.save("./tmp/normalmap.bmp")


if __name__ == "__main__":
    main()
