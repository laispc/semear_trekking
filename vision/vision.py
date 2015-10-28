#!/usr/bin/python

import cv2
import numpy as np
import math
import serial
import time

#-----------------------CALIBRATION--------------------------#
# Serial setting
B=9600
# White area detection
WIDTH=320
HEIGHT=240
HMIN=0
HMAX=240
TMIN=100
TMAX=200

# Define view areas
LOWER_VIEW = HEIGHT - 50
GAP = 20
ALFA = 70
ALFA_RADS = float(1.0*ALFA/180*math.pi)	#degrees

CENTER_VIEW = 640/2
LEFT_VIEW = CENTER_VIEW - 80
RIGHT_VIEW = CENTER_VIEW + 80 
#------------------------------------------------------------#
def answer(R):
	ser.write(str(R))

def region(cx,cy):
	if ((cx == -1 or cy == -1) or (cx >= WIDTH - GAP - (cy/math.tan(ALFA)))): 
		return "R"
	if (cy > LOWER_VIEW and cx < WIDTH - GAP - (cy/(math.tan(ALFA))) and cx > GAP + (cy/(math.tan(ALFA))) ): 
		return "S"
	if (cx <= GAP + (cy/math.tan(ALFA))): 
		return "L"
	return "F"

rcv = ""
ser = serial.Serial('/dev/ttyS0', B, timeout=0)
#ser.open();
ser.flushInput()
log = open('camlog.txt','a')
log.write("\n\n\nStart at "+time.strftime("%d-%m-%Y, %H:%M:%S")+"\n\n")

cap=cv2.VideoCapture(0)
cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,WIDTH)
cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,HEIGHT)

while 1:
	q = ser.read(1)
	if q == "S":
		break
	
	if q == "Q":
		ret,frame = cap.read()
		if ret:
			cframe = frame[HMIN:HMAX, 1:WIDTH]
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
					log.write(time.strftime("%d-%m-%Y, %H:%M:%S")+"Error(1): cannot calculate centroid")
			else:
				cx = -1
				cy = -1
			R = region(cx,cy)
			answer(R)
			log.write(time.strftime("%d-%m-%Y, %H:%M:%S")+"cx="+str(cx)+", cy="+str(cy)+", region="+str(R)+"\n")
			cv2.imwrite("pic_"+str(time.time())+"_cframe.jpg",edges)
			cv2.imwrite("pic_"+str(time.time())+"_edges.jpg",edges)
		else:
			log.write(time.strftime("%d-%m-%Y, %H:%M:%S")+"Error(0): Problem with cam")
		q = ""

ser.close()
cap.release();
