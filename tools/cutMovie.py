from moviepy.editor import *
import argparse


parser = argparse.ArgumentParser(
    description='Cut video from another file from begin frame to end frame.',
    prog='cutMovie', usage='%(prog)s -i <input file> -o <output file> --begin <begin frame number> --end <end frame number>',
    exit_on_error=False)
parser.add_argument('-i', '--inputfile', type=str, required=True,
                    help='path of the input file')
parser.add_argument('-o', '--outputfile', type=str, required=True,
                    help='path of the output file')
parser.add_argument('-b', '--begin', type=int, required=True,
                    help='begin frame')
parser.add_argument('-e', '--end', type=int, required=True,
                    help='end frame')

try:
    args = parser.parse_args()
except:
    parser.print_help()
    exit()


clip = VideoFileClip(args.inputfile)
clip = clip.subclip(args.begin / clip.fps, args.end / clip.fps)
clip.write_videofile(args.outputfile)
