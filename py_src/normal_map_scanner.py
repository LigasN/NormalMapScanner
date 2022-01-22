import numpy as np
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


def load_image(filename):
    try:
        return Image.open(
            assets_path + filename)
    except FileNotFoundError:
        print('File not found: ' + assets_path + filename)


def main():
    input_images = np.empty(angles.size, dtype=BitmapImage)
    output_image = Image.new('RGB', image_size.tolist())

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
    # TODO: probably just right away counting right pixel for normal map
    pixel_size = object_size / image_size
    # Calculate vector pointing to the light source per pixel
    for pixel_index in np.ndindex(image_size[x], image_size[y]):
        # (pixel position for an object aligned with its upper left corner
        # to (0,0)) -/+ to center results back)
        pixel_pos = ((pixel_index * pixel_size) +
                     (np.array(pixel_size[x], -pixel_size[y]) / 2)) - (np.array(object_size[x], -object_size[y]) / 2)
        pixel_pos = np.append(pixel_pos, 0.)  # Z axis
        L_vector = lamp_pos[0] - pixel_pos
    # Convert back to PIL
    #Image.fromarray(arrOut, mode='RGB').show()

    # Create output image object
    # output_image.show()
    print('Saving')
    # output_image.save("./tmp/normalmap.bmp")


if __name__ == "__main__":
    main()
