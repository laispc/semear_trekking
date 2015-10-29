#!/usr/bin/python

import cv2
import numpy as np
import math
import serial
import time

log = open('camlog.txt','a')
log.write("\n\n\nStart at "+time.strftime("%d-%m-%Y, %H:%M:%S")+"\n\n")

ser = serial.Serial('/dev/ttyS0', 9600, timeout=0)
ser.flushInput()

#-----------------------CALIBRATION--------------------------#
# White area detection
WIDTH=0
HEIGHT=0
HMIN=0
HMAX=0
TMIN=0
TMAX=0

# Define view areas
LOWER_VIEW = 0
GAP = 0
ALFA = 0.0
ALFA_RADS = 0.0

CENTER_VIEW = 0
LEFT_VIEW = 0
RIGHT_VIEW = 0
#------------------------------------------------------------#

def settings():
	#get calibration constants as "320,240,000,240,100,200,050,020,070,080,080,0,." (len=47)
	c = "";
	while 1:
		c = ser.read(47)
		cal = c.split(',')
		if cal[-1] == ".":
			WIDTH=int(cal[0])
			HEIGHT=int(cal[1])
			HMIN=int(cal[2])
			HMAX=int(cal[3])
			TMIN=int(cal[4])
			TMAX=int(cal[5])
			LOWER_VIEW=int(cal[6])
			GAP=int(cal[7])
			ALFA=float(cal[8])
			ALFA_RADS=float(1.0*ALFA/180*math.pi)
			CENTER_VIEW=WIDTH/2
			LEFT_VIEW=int(cal[9])
			RIGHT_VIEW=int(cal[10])
			IMGSAVE=int(cal[11])
			log.write(time.strftime("%d-%m-%Y, %H:%M:%S")+"Configuration: "+c)
			cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,WIDTH)
			cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,HEIGHT)
			break

def region(cx,cy):
	if ((cx == -1 or cy == -1) or (cx >= WIDTH - GAP - (cy/math.tan(ALFA)))): 
		return "R"
	if (cy > LOWER_VIEW and cx < WIDTH - GAP - (cy/(math.tan(ALFA))) and cx > GAP + (cy/(math.tan(ALFA))) ): 
		return "S"
	if (cx <= GAP + (cy/math.tan(ALFA))): 
		return "L"
	return "F"

cap=cv2.VideoCapture(-1)

q=""
while 1:
	q = ser.read(1)
	if q == "S":
		break
	
	if q == "C":
		settings()
	
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
			log.write(time.strftime("%d-%m-%Y, %H:%M:%S")+"cx="+str(cx)+", cy="+str(cy)+", region="+str(R)+"\n")
			if IMGSAVE==1:
				cv2.imwrite("pic_"+str(time.time())+"_cframe.jpg",edges)
				cv2.imwrite("pic_"+str(time.time())+"_edges.jpg",edges)
			ser.write(str(R))
		else:
			log.write(time.strftime("%d-%m-%Y, %H:%M:%S")+"Error(0): Problem with reading")
		q = ""

ser.close()
cap.release();
