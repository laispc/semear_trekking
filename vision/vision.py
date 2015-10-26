#!/usr/bin/python

import cv2
import numpy as np

XRES=320
YRES=240
HMIN=0
HMAX=240
TMIN=100
TMAX=200

cap=cv2.VideoCapture(0)
cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,XRES)
cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,YRES)


#start from here when event from arduino
ret,frame = cap.read()
if ret:
	cframe = frame[HMIN:HMAX, 1:XRES]
	gray = cv2.cvtColor(cframe, cv2.COLOR_BGR2GRAY)	
	_,gray = cv2.threshold(gray,TMAX,TMIN,0)
	
	edges = cv2.Canny(gray,100,200)
	kernel = np.ones((5,5),np.float32)/10
	edges = cv2.filter2D(edges,-1,kernel)
	
	cnts,_ = cv2.findContours(edges.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
	cnts = sorted(cnts, key = cv2.contourArea, reverse = True)
	
	if len(cnts):
		cnt = cnts[0] #choose biggest contour
		M = cv2.moments(cnt)
		if M['m00']:
			cx = int(M['m10']/M['m00'])
			cy = int(M['m01']/M['m00'])
		else:
			print "Error(1): cannot calculate centroid"
	else:
		cx = -1
		cy = -1
	
	#cx,cy -> angle,distance
	
else:
	print "Error(3): Problem with cam"


	
#cv2.imwrite("cap.jpg",edges)
cap.release();
