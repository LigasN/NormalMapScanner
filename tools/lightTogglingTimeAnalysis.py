from moviepy.editor import *


# clip = VideoFileClip("tmp/my_video.mp4")
#video = CompositeVideoClip([clip])
# video.write_videofile("tmp/myHolidays_edited.mp4")

import cv2
import numpy as np
import matplotlib.pyplot as plt


def grabFrame(cap, frame):
    ret, img = cap.read()

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    show_size = (50, 20)
    gray_show = cv2.resize(gray, show_size)
    #text_pos = (20,10)
    #gray_show = cv2.putText(gray_show, "frame: " + str(frame), text_pos,
    #                        cv2.FONT_HERSHEY_SIMPLEX, 0.2, (255, 255, 255), 2, cv2.LINE_AA)

    hist = cv2.calcHist([gray], [0], None, [256], [0, 256])
    plt.title("Histogram" + str(frame))

    return hist, cv2.cvtColor(gray_show, cv2.COLOR_GRAY2RGB)


cap = cv2.VideoCapture('tmp/my_video.mp4')

fig = plt.figure(figsize=(8, 4))

hist, vid = grabFrame(cap, 0)
# create two subplots
ax1 = plt.subplot(121)
ax2 = plt.subplot(122)
plt.ion()
frame = 0
while(cap.isOpened()):
    hist, vid = grabFrame(cap, frame)
    ax1.imshow(vid)
    ax2.plot(hist)
    plt.pause(0.00001)
    frame += 1
plt.ioff()
plt.show()

cap.release()
