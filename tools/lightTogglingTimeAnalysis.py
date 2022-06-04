import cv2
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.style as mplstyle

mplstyle.use('fast')
cap = cv2.VideoCapture('tmp/test1.mp4')
fps = int(cap.get(cv2.CAP_PROP_FPS))
framesAmount = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))

fig = plt.figure(figsize=(8, 4))

# create two subplots
ax1 = plt.subplot(121)
ax1.text(0, -2, "Fps: " + str(fps))
ax2 = plt.subplot(122)
    
show_scale = 0.05

frame = 0
while(cap.isOpened()):
    plt.title("Frame: " + str(frame) + '/' + str(framesAmount))
    ret, img = cap.read()
    if not ret:
        break
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    show_size = (int(img.shape[1] * show_scale), int(img.shape[0] * show_scale))
    gray_show = cv2.resize(gray, show_size)
    ax1.imshow(cv2.cvtColor(gray_show, cv2.COLOR_GRAY2RGB))

    hist = cv2.calcHist([gray], [0], None, [256], [0, 256])
    ax2.plot(hist)
    plt.pause(0.00001)
    frame += 1

plt.show()

cap.release()
