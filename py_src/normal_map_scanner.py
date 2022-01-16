from PIL import Image


R, G, B = 0, 1, 2


def load_image_BMP2RGB(filepath, filename):
    im = Image.open("../assets/input_0.bmp")
    im.split()


def main():
    load_image_BMP2RGB("../assets /", "input_0.bmp")


if __name__ == "__main__":
    main()
