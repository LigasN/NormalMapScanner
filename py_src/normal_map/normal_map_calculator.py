import argparse
import os
import sys
import numpy as np
import time
from PIL import Image, ImageChops
import datetime


class NormalMapCalculator:
    """Calculates normal map texture.

    Args:
        input files directory (str): input files directory
        input file name prefix (str): input file name prefix
        output file location (str): output file in relative path
        image size x axis (int): input and output images pixels on x axis size
        image size y axis (int): input and output images pixels on y axis size
        object size x axis (int): object size in centimeters on x axis
        object size y axis (int): object size in centimeters on y axis
        lamp 0 position x axis (int): position of the first lamp in cm (x axis)
        lamp 0 position y axis (int): position of the first lamp in cm (y axis)
        lamp 0 position z axis (int): position of the first lamp in cm (z axis)
    """

    def __init__(self, source_files, object_size, lamp_0_position, environment_light):
        self.source_files = source_files
        self.object_size = object_size
        self.lamp_0_position = lamp_0_position
        self.image_size = source_files[0].shape
        self.environment_light = environment_light

    def __printProgressBar(self, iteration, prefix='', suffix='', decimals=1,
                           length=50, fill='█', printEnd="\r"):
        """
        Call in a loop to create terminal progress bar
        @params:
            iteration   - Required  : current iteration (Int)
            prefix      - Optional  : prefix string (Str)
            suffix      - Optional  : suffix string (Str)
            decimals    - Optional  : number of decimals in percent (Int)
            length      - Optional  : character length of bar (Int)
            fill        - Optional  : bar fill character (Str)
            printEnd    - Optional  : end character (e.g. "\r", "\r\n") (Str)
        """
        percent = ("{0:." + str(decimals) + "f}").format(
            100 * (iteration / float(self.image_size[1])))
        filledLength = int(length * iteration // self.image_size[1])
        bar = fill * filledLength + '-' * (length - filledLength)
        print(f'\r{prefix} |{bar}| {percent}% {suffix}', end=printEnd)

    def __sRGB2Linear(self, im):
        """
        Converts image stored in convertable to np.array object from sRGB 
        color mapping space to linear.

        @params:
            im   - Required  : image stored in convertable to np.array object
        """
        # Convert to numpy array of float
        arr = np.array(im, dtype=np.float32) / 255.0
        # Convert sRGB -> linear
        arr = np.where(arr <= 0.04045, arr/12.92, ((arr+0.055)/1.055)**2.4)
        # Convert to 8-bit
        arrOut = np.uint8(np.rint(arr * 255.0))
        # Convert back to PIL
        return Image.fromarray(arrOut)

    def __magnitude(self, v):
        return np.sqrt(np.sum(v**2))

    def __normalize(self, v):
        mag = self.__magnitude(v)
        if(mag == 0.0):  # prevents dividing by zero
            # NINF value suits calculations with images
            return np.full(np.shape(v), -np.NINF)
        return v / self.magnitude(v)

    def load_image(self, filename):
        try:
            return Image.open(
                filename)
        except FileNotFoundError:
            print('File not found: ' + filename)

    def calculateNormalMap(self, verbose=False, log=False):
        R, G, B = 0, 1, 2
        X, Y, Z = 0, 1, 2
        angles = np.array(range(0, 360, 45))
        angles_rad = np.array((angles * np.pi) / 180, float)

        # Creating tmp directory for future purposes
        if not os.path.exists("./tmp/"):
            os.makedirs("./tmp/")

        # Logging initialization
        if (log):
            sys.stdout = open('./tmp/log.txt', 'w')
            a = datetime.datetime.now()
            print(str(datetime.datetime.now()) + '\n')

        # single pixel stores sum of RGB colors
        input = np.zeros((8, self.image_size[1], self.image_size[0]), float)
        output = np.zeros((self.image_size[1], self.image_size[0], 3), float)

        for i in range(0, angles.size):
            if (verbose):
                print('Loading of the input data array with angle: %d' %
                      angles[i])

            start_time = time.time()

            # Subtract environment light
            im = ImageChops.subtract(
                im, self.environment_light)

            # Sum colors
            input[i] = (np.array(im.getdata(R), float) +
                        np.array(im.getdata(G), float) +
                        np.array(im.getdata(B), float)).reshape(
                self.image_size[1], self.image_size[0])

            if (verbose):
                print("Loaded and converted in %d ms" %
                      ((time.time() - start_time) * 1000))

        if (verbose):
            print('Calcultion of the positions of the lamps')

        start_time = time.time()
        lamp_pos = np.empty((8, 3))
        lamp_pos[0] = self.lamp_0_position
        lamp_distance = self.lamp_0_position[0]
        for angle_idx in range(1, angles_rad.size):
            # Calculate every lamp position (z position is not changing)
            lamp_pos[angle_idx][0] = np.cos(
                angles_rad[angle_idx]) * lamp_distance
            lamp_pos[angle_idx][1] = np.sin(
                angles_rad[angle_idx]) * lamp_distance
            lamp_pos[angle_idx][2] = self.lamp_0_position[2]

        if(verbose):
            print("Position of lamps calculated in %d ms" %
                  ((time.time() - start_time) * 1000))
            print(lamp_pos)
            print('Calculation of the normalmap vectors')

        start_time = time.time()
        pixel_size = self.object_size / self.image_size
        pixel_idx = np.zeros(2)

        for y, x in np.ndindex(self.image_size[1], self.image_size[0]):

            if (verbose):
                self.__printProgressBar(y + 1)

            pixel_idx = np.array((x, y))
            # Pixel position calculation is at first calculated for an object
            # aligned with its bottom left corner to (0, 0)). However Pillow
            # library is storing images aligned with its upper left corner to
            # the (0, 0) point. So after first calculations the value of
            # pixel_pos variable is set on (x, -y, 0) to correct the equations
            # for Pillow alignment.
            pixel_pos = ((pixel_idx * pixel_size) +
                         (pixel_size / 2) -
                         (self.object_size / 2))
            pixel_pos = np.array((pixel_pos[0], -pixel_pos[1], 0.))

            N_vector = np.zeros(3, float)
            for angle_idx in range(angles_rad.size):
                # Vector pointing to the light source
                L_vector = lamp_pos[angle_idx] - pixel_pos
                # L_vector is pointing to the lamp. Input image for normal vector
                # equal to this should be white (255,255,255). For opposite
                # vector color would be black (0,0,0).
                weight = input[angle_idx][y][x] / 765.0
                N_vector += L_vector * weight

                if(verbose):
                    print("pixel idx [%d, %d]" % (x, y))
                    print("angle idx: %d" % angle_idx)
                    print("pixel size")
                    print(pixel_size)
                    print("pixel pos")
                    print(pixel_pos)
                    print("Lamp pos")
                    print(lamp_pos[angle_idx])
                    print("L vector")
                    print(L_vector)
                    print("pixel color")
                    print(input[angle_idx][y][x])
                    print("weight")
                    print(weight)
                    print("N_vector")
                    print(L_vector * weight)
                    print("N_vector after addition")
                    print(N_vector)

            output[y][x] = self.__normalize(N_vector)

            if(verbose):
                print("Output")
                print(output[y][x])

        if(verbose):
            print("\nNormalmap vectors calculated in %d ms" %
                  ((time.time() - start_time) * 1000))
            print('Conversion back to PIL')

        # Create output image object
        start_time = time.time()
        image_data = np.array((((output + 1.0) / 2.0) * 255.0), np.uint8)
        normalmap = Image.fromarray(image_data, mode='RGB')

        if (verbose):
            print("Convertion back to PIL done in %d ms" %
                  ((time.time() - start_time) * 1000))
        if (log):
            sys.stdout.close()  # Close logging

        return normalmap


def main():
    # Script arguments parser
    parser = argparse.ArgumentParser()
    # -id input files directory -o output file -p PASSWORD -size 20000
    parser.add_argument("-i", "--input_directory", dest="assets_path",
                        default="test_input/4.blender_1200x1200px/", help="input files directory (str)")
    parser.add_argument("-in", "--input_filename_prefix",
                        dest="input_filename_prefix", default="input_", help="input file name prefix (str)")
    parser.add_argument("-o", "--output", dest="output_file",
                        default="./tmp/normalmap.bmp", help="output file in relative path (str)")

    parser.add_argument("-isx", "--image_size_x", dest="image_size_x", default=1200, type=int,
                        help="image size x axis (int): input and output images pixels on x axis size")
    parser.add_argument("-isy", "--image_size_y", dest="image_size_y", default=1200, type=int,
                        help="image size y axis (int): input and output images pixels on y axis size")

    parser.add_argument("-osx", "--object_size_x", dest="object_size_x", default=20,
                        type=int, help="object size x axis (int): object size in centimeters on x axis")
    parser.add_argument("-osy", "--object_size_y", dest="object_size_y", default=20,
                        type=int, help="object size y axis (int): object size in centimeters on y axis")

    parser.add_argument("-lpx", "--lamp_0_position_x", dest="lamp_0_pos_x", default=40, type=int,
                        help="lamp 0 position x axis (int): position of the first lamp in cm (x axis)")
    parser.add_argument("-lpy", "--lamp_0_position_y", dest="lamp_0_pos_y", default=0, type=int,
                        help="lamp 0 position y axis (int): position of the first lamp in cm (y axis)")
    parser.add_argument("-lpz", "--lamp_0_position_z", dest="lamp_0_pos_z", default=15, type=int,
                        help="lamp 0 position z axis (int): position of the first lamp in cm (z axis)")

    parser.add_argument("-v", "--verbose", action='store_true', default=False,
                        help="print all logs")
    parser.add_argument("-l", "--log", action='store_true', default=False,
                        help="Save all logs in ./tmp/log.txt")

    args = parser.parse_args()


if __name__ == "__main__":
    main()
