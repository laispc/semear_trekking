#!/usr/bin/python

import cv2
import numpy as np
import time

cap=cv2.VideoCapture(0)

cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,320)
cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,240)

t = time.time()
#cap.grab() #0.25 sec -> 4 frames/sec
#ret,frame=cap.retrieve(0)
ret,frame = cap.read()
print 1/(time.time()-t), " FPS"



print cap.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH)
#print cap.get(cv2.cv.CV_CAP_PROP_EXPOSURE)

cv2.imwrite("cap.jpg",frame)
cap.release()

