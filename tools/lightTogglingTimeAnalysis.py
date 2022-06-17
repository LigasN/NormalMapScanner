import cv2
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.style as mplstyle
import matplotlib.ticker as ticker


mplstyle.use('fast')
cap = cv2.VideoCapture('tmp/my_video.mp4')
fps = int(cap.get(cv2.CAP_PROP_FPS))
framesAmount = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))

# log values
file = open("tmp\MeanColorValues.txt", "w")
file.write("frame\tmean color value\n")

# create three subplots
"""
fig = plt.figure(figsize=(12, 4))
plt.subplots_adjust(wspace=0.5, bottom=0.2)

ax1 = plt.subplot(131)
ax1.text(0, -2, "Fps: " + str(fps))
ax2 = plt.subplot(132)
ax2.set_xlabel("Color value")
ax2.set_ylabel("Amount of pixels")
ax3 = plt.subplot(133)
ax3.set_xlabel("Frame")
ax3.set_ylabel("Mean value of color")
line = plt.Line2D([], [], color='black')
line_xdata = np.arange(framesAmount)
ax3.add_line(line)
ax3.set_xlim(0, framesAmount)
ax3.set_ylim(0, 255)
ax3.grid()
"""
# create two subplots
fig = plt.figure(figsize=(12, 8))
plt.subplots_adjust(wspace=0.5, bottom=0.2)

ax1 = plt.subplot(211)
ax1.text(0, -2, "Fps: " + str(fps))
ax3 = plt.subplot(212)
ax3.set_xlabel("Frame")
ax3.set_ylabel("Mean value of color")
line = plt.Line2D([], [], color='black')
line_xdata = np.arange(framesAmount)
ax3.add_line(line)
ax3.set_xlim(0, framesAmount)
ax3.set_ylim(0, 255)
ax3.grid()

plt.ioff()

show_scale = 0.05

# Array for mean values of colors of each frame
meanArray = np.zeros(framesAmount)

frame = 0
while(cap.isOpened()):
    plt.title("Frame: " + str(frame) + '/' + str(framesAmount))
    ret, img = cap.read()
    if not ret:
        break
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    #show_size = (int(img.shape[1] * show_scale),
    #             int(img.shape[0] * show_scale))
    #gray_show = cv2.resize(gray, show_size)
    #ax1.imshow(cv2.cvtColor(gray_show, cv2.COLOR_GRAY2RGB))

    #hist = cv2.calcHist([gray], [0], None, [256], [0, 256])
    #ax2.plot(hist)

    meanArray[frame] += gray.mean()
    line.set_data(line_xdata[:frame], meanArray[:frame])
    
    file.write(str(frame) + '\t\t' + str(meanArray[frame]) + '\n')

    fig.canvas.draw()
    plt.pause(0.00001)
    frame += 1

file.close()
cap.release()
exit()
