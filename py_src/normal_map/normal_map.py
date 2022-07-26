import argparse
import os
import sys
import numpy as np
import time
from PIL import Image, ImageChops
import datetime


class NormalMap:
    """Calculates normal map texture.

    Args:
        source files: dictionary(int : PIL:Image) - source files given as
            a dictionary with key equal to value of the angle of the light
            during making a picture and already loaded Image.
        object size: tuple[int] - size of the examined object in milimeters
            given as a 2-tuple (width, height).
        lamp 0 position: tuple[int] - position of the first lamp of the stand
            in milimeters given as a 3-tuple (x-axis, y-axis, z-axis).
        environment light: PIL:Image - already loaded Image of the environment
            light (no stand lights turned on during making this picture).
    """
    R, G, B = 0, 1, 2
    X, Y, Z = 0, 1, 2
    angles = np.array(range(0, 360, 45))
    angles_rad = np.array((angles * np.pi) / 180, float)

    def __init__(
        self, source_files, object_size, lamp_0_position,
            environment_light):
        self.source_files = source_files
        self.object_size = np.array(object_size)
        self.lamp_0_position = np.array(lamp_0_position)
        self.image_size = np.array(source_files[0].size)
        self.environment_light = environment_light
        self.normalmap = None
        self.logfile = None

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

    def __deg2Rad(self, angle):
        return (angle * np.pi) / 180,

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
        return v / self.__magnitude(v)

    def __log(self, message):
        print(message)
        if self.logfile: 
            self.logfile.write(message)

    def calculateNormalMap(self, verbosity=0, log=False, progressbar=True):
        # Creating tmp directory for future purposes
        if not os.path.exists("./tmp/"):
            os.makedirs("./tmp/")

        # Logging initialization
        if (log):
            self.logfile = open('./tmp/log.txt', 'w')
            self.__log(str(datetime.datetime.now()) + '\n')

        # single pixel stores sum of RGB colors
        input = dict()
        output = np.zeros((self.image_size[1], self.image_size[0], 3), float)

        for angle in self.angles:
            if verbosity >= 1:
                self.__log('Loading of the input data array with angle: %d' %
                      angle)
                start_time = time.time()

            # Subtract environment light
            im = ImageChops.subtract(
                self.source_files[angle], self.environment_light)

            # Sum colors
            input[angle] = (np.array(im.getdata(self.R), float) +
                        np.array(im.getdata(self.G), float) +
                        np.array(im.getdata(self.B), float)).reshape(
                self.image_size[1], self.image_size[0])

            if (verbosity >= 1):
                self.__log("Loaded and converted in %d ms" %
                      ((time.time() - start_time) * 1000))

        if (verbosity >= 1):
            self.__log('Calcultion of the positions of the lamps')

        start_time = time.time()
        lamp_pos = { self.angles[0]: self.lamp_0_position }
        lamp_distance = self.lamp_0_position[0]
        for angle in self.angles[1:self.angles.size]:
            # Calculate every lamp position (z position is not changing)
            lamp_pos[angle] = np.array([
                float(np.cos(self.__deg2Rad(angle)) * lamp_distance),
                float(np.sin(self.__deg2Rad(angle)) * lamp_distance),
                float(self.lamp_0_position[2])])

        if verbosity >= 1:
            self.__log("Position of lamps calculated in %d ms" %
                  ((time.time() - start_time) * 1000))
            self.__log('Calculation of the normalmap vectors')

        if verbosity >= 2:
            self.__log(lamp_pos)

        start_time = time.time()
        pixel_size = self.object_size / self.image_size
        pixel_idx = np.zeros(2)

        for y, x in np.ndindex(self.image_size[1], self.image_size[0]):

            if (verbosity >= 1 or progressbar):
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
            for angle in self.angles:
                # Vector pointing to the light source
                L_vector = lamp_pos[angle] - pixel_pos
                # L_vector is pointing to the lamp. Input image for normal
                # vector equal to this should be white (255,255,255). For
                # opposite vector color would be black (0,0,0).
                weight = input[angle][y][x] / 765.0
                N_vector += L_vector * weight

                if verbosity >= 2:
                    self.__log("pixel idx [%d, %d]" % (x, y))
                    self.__log("angle: %d" % angle)
                    self.__log("pixel size")
                    self.__log(pixel_size)
                    self.__log("pixel pos")
                    self.__log(pixel_pos)
                    self.__log("Lamp pos")
                    self.__log(lamp_pos[angle])
                    self.__log("L vector")
                    self.__log(L_vector)
                    self.__log("pixel color")
                    self.__log(input[angle][y][x])
                    self.__log("weight")
                    self.__log(weight)
                    self.__log("N_vector")
                    self.__log(L_vector * weight)
                    self.__log("N_vector after addition")
                    self.__log(N_vector)

            output[y][x] = self.__normalize(N_vector)

            if verbosity >= 2:
                self.__log("Output")
                self.__log(output[y][x])

        if verbosity >= 1:
            self.__log("\nNormalmap vectors calculated in %d ms" %
                  ((time.time() - start_time) * 1000))
            self.__log('Conversion back to PIL')

        # Create output image object
        start_time = time.time()
        image_data = np.array((((output + 1.0) / 2.0) * 255.0), np.uint8)
        self.normalmap = Image.fromarray(image_data, mode='RGB')

        if verbosity >= 1:
            self.__log("Convertion back to PIL done in %d ms" %
                  ((time.time() - start_time) * 1000))
        if (log):
            self.logfile.close()  # Close logging

    def isNormalMapReady(self):
        return self.normalmap != None and self.normalmap.size > [0, 0]


if __name__ == "__main__":

    def load_image(filename):
        try:
            return Image.open(
                filename)
        except FileNotFoundError:
            print('File not found: ' + filename)

    # Script arguments parser
    parser = argparse.ArgumentParser()
    # -id input files directory -o output file -p PASSWORD -size 20000
    parser.add_argument("-i", "--input_directory", dest="assets_path",
                        default="test_input/4.blender_1200x1200px/",
                        help="input files directory (str)")
    parser.add_argument("-in", "--input_filename_prefix",
                        dest="input_filename_prefix", default="input_",
                        help="input file name prefix (str)")
    parser.add_argument("-en", "--environment_filename",
                        dest="environment_filename", default="all_off",
                        help="""enviroment light image name after 
                        --input_filename_prefix (str)""")
    parser.add_argument("-o", "--output", dest="output_file",
                        default="./tmp/normalmap.bmp",
                        help="output file in relative path (str)")

    parser.add_argument(
        "-osx", "--object_size_x", dest="object_size_x", default=20, type=int,
        help="object size x axis (int): object size in centimeters on x axis")
    parser.add_argument(
        "-osy", "--object_size_y", dest="object_size_y", default=20, type=int,
        help="object size y axis (int): object size in centimeters on y axis")

    parser.add_argument(
        "-lpx", "--lamp_0_position_x", dest="lamp_0_pos_x", default=40,
        type=int,
        help="""lamp 0 position x axis (int): position of the first lamp in cm 
        (x axis)""")
    parser.add_argument(
        "-lpy", "--lamp_0_position_y", dest="lamp_0_pos_y", default=0,
        type=int,
        help="""lamp 0 position y axis (int): position of the first lamp in cm 
        (y axis)""")
    parser.add_argument(
        "-lpz", "--lamp_0_position_z", dest="lamp_0_pos_z", default=15,
        type=int,
        help="""lamp 0 position z axis (int): position of the first lamp in cm
        (z axis)""")

    parser.add_argument(
        "-v", "--verbosity", dest="verbosity", action="count", default=0,
        help="""print all logs. -v print only status logs, -vv print all debug 
        information""")
    parser.add_argument(
        "-l", "--log", dest="log", action='store_true', default=False,
        help="Save all logs in ./tmp/log.txt")
    parser.add_argument(
        "-p", "--no_progress_bar", dest="no_progress_bar", 
        action='store_true', default=False, help="Turns off progress bar.")

    args = parser.parse_args()

    output_path = os.path.dirname(os.path.abspath(args.output_file))
    if not os.path.exists(output_path):
        os.makedirs(output_path, exist_ok=True)

    sourcefiles = dict()
    for angle in NormalMap.angles:
        sourcefiles[angle] = load_image(
            args.assets_path + args.input_filename_prefix + str(angle) +
            '.bmp')

    environment_light = load_image(
        args.assets_path + args.input_filename_prefix +
        args.environment_filename + '.bmp')

    normalmap = NormalMap(source_files=sourcefiles,
                          object_size=(args.object_size_x, args.object_size_y),
                          lamp_0_position=(args.lamp_0_pos_x,
                                           args.lamp_0_pos_y,
                                           args.lamp_0_pos_z),
                          environment_light=environment_light)

    normalmap.calculateNormalMap(verbosity=args.verbosity, log=args.log, 
        progressbar=(not args.no_progress_bar))

    if args.verbosity >= 1:
        normalmap.normalmap.show()
        print('Saving')
        
    normalmap.normalmap.save(args.output_file)
