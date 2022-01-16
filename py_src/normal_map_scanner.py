import numpy as np
from tkinter import BitmapImage
from PIL import Image


R, G, B = 0, 1, 2
angle = np.array(range(0, 360, 45))


def main():
    # Load input images
    input_images = np.empty(angle.size, dtype=BitmapImage)
    for i in range(0, angle.size):
        input_images[i] = (Image.open(
            "../assets/input_" + str(angle[i]) + ".bmp"))

    # Save output image
    output = input_images[3]
    output.show()
    output.save("./normalmap.bmp")


if __name__ == "__main__":
    main()
