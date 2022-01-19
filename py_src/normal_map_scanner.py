import numpy as np
from tkinter import BitmapImage
from PIL import Image


R, G, B = 0, 1, 2
angle = np.array(range(0, 360, 45))
Light_intenisty_W = 2  # Watts
Light_intenisty_lumens = 220  # lumens


def main():
    print('Loading input images.')
    input_images = np.empty(angle.size, dtype=BitmapImage)
    for i in range(0, angle.size):
        try:
            input_images[i] = Image.open(
                '../assets/input_' + str(angle[i]) + '.bmp')
        except FileNotFoundError:
            print('File not found: ../assets/input_' + str(angle[i]) + '.bmp')

    # Create output image object
    output = Image.new('RGB', (1200, 1200))
    angles = np.empty(1)

    for i in range(0, angle.size):
        angles[i] = np.arccos()
    print(angles)
    # Save output image
    # output.show()
    print('Saving')
    # output.save("./tmp/normalmap.bmp")


if __name__ == "__main__":
    main()
