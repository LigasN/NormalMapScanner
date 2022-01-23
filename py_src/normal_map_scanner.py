import numpy as np
import math
from tkinter import BitmapImage
from PIL import Image, ImageChops

# Properties
image_size = np.array([1200, 1200])  # px
object_size = np.array([20, 20])  # cm
assets_path = '../assets/'
lamp_0_position = np.array([40, 0, 15])  # cm

r, g, b = 0, 1, 2
x, y, z = 0, 1, 2
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
    return math.sqrt(vec2[x]**2 + vec2[y]**2)


def load_image(filename):
    try:
        return Image.open(
            assets_path + filename)
    except FileNotFoundError:
        print('File not found: ' + assets_path + filename)


def main():
    input_images = np.empty(angles.size, dtype=BitmapImage)
    out_image = np.zeros((1200, 1200, 3), float)

    print('Loading of the input images.')
    for i in range(0, angles.size):
        input_images[i] = load_image('input_' + str(angles[i]) + '.bmp')
    environment_light = load_image(
        '../assets/input_all_off.bmp')

    print('Subtraction of the evironment light from input images')
    for i in range(0, angles.size):
        input_images[i] = ImageChops.subtract(
            input_images[i], environment_light)

    print('Calcultion of the positions of the lamps')
    lamp_pos = np.empty((8, 3))
    lamp_pos[0] = lamp_0_position
    lamp_distance = lamp_0_position[x]
    for angle_idx in range(angles_rad.size):
        # Calculate every lamp position (z position is not changing)
        if angles_rad[angle_idx] != 0:
            lamp_pos[angle_idx][x] = np.cos(
                angles_rad[angle_idx]) * lamp_distance
            lamp_pos[angle_idx][y] = np.sin(
                angles_rad[angle_idx]) * lamp_distance
            lamp_pos[angle_idx][z] = lamp_0_position[z]

    print('Calculation of the ...')
    pixel_size = object_size / image_size
    pixel_idx = np.zeros(2)
    for pixel_idx[y], pixel_idx[x] in np.ndindex(image_size[y], image_size[x]):
        # (pixel position for an object aligned with its upper left corner
        # to (0,0)) -/+ to center results back)
        pixel_pos = ((pixel_idx * pixel_size) +
                     (np.array((pixel_size[x], (-pixel_size[y]))) / 2)) - (np.array((object_size[x], (-object_size[y]))) / 2)
        pixel_pos = np.append(pixel_pos, 0.)  # Z axis

        N_vectors = np.zeros((8, 3))
        for angle_idx in range(angles_rad.size):
            # Vector pointing to the light source
            L_vector = lamp_pos[angle_idx] - pixel_pos
            # L_vector is pointing to the lamp. Input image for normal vector equal to this should be white (255,255,255). For opposite vector color would be black (0,0,0). So calculate te percentage value of color in input image and calculate the angle from this.
            pix = input_images[angle_idx].getpixel(
                (pixel_idx[x], pixel_idx[y]))
            pix_value = float(np.sum(pix))
            weight = pix_value / 765.
            NL_angle = (1 - weight) * np.pi
            # To move a vector without using quaterions it is moved on its axis created parallel to diagonal of X and Y values of the vector
            diagonal = vec2_mag(L_vector[:z])
            dz_mag = vec2_mag((diagonal, L_vector[z]))
            new_diagonal = np.cos(
                NL_angle) * dz_mag  # x axis
            N_vectors[angle_idx][z] = np.sin(
                NL_angle) * dz_mag  # y axis
            # Diagonal change ratio to get x and y back
            diag_ratio = new_diagonal / diagonal
            N_vectors[angle_idx][:z] = diag_ratio * L_vector[:z]
        # Averaging of the N vector results
        N_vector = np.average(N_vectors, axis=0)
        out_image[int(pixel_idx[y])][int(pixel_idx[x])][x] = N_vector[x]

    # Convert back to PIL
    normalmap = Image.fromarray(out_image, mode='RGB')

    # Create output image object
    normalmap.show()
    print('Saving')
    normalmap.save("./tmp/normalmap.bmp")


if __name__ == "__main__":
    main()
