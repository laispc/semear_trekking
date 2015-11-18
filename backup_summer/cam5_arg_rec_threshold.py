#!/usr/bin/python

import cv2
import numpy as np
import math
import serial
import time

ser = serial.Serial('/dev/ttyS0', 19200, timeout=0)
ser.flushInput()

#-----------------------CALIBRATION--------------------------#
# White area detection
WIDTH=320
HEIGHT=240
HMIN=0
HMAX=240
THRESHIGHLIGHT=255
THRES=230

# Define view areas
LOWER_VIEW = 190
#GAP = int(WIDTH * (20.0/100))
GAP = 50
ALFA = 70.0
ALFA_RADS = ALFA/180*math.pi

IMGSAVE=0
#------------------------------------------------------------#

# This function was used to set the parameters received from arduino
# No longer used due to problems in serial communication
'''
def settings():
	#get calibration constants as "320,240,000,240,100,200,050,020,070,0,." (len=39)
	
	c = "";
	while 1:
		c = ser.read(39)
		cal = c.split(',')
		if cal[-1] == ".":
			print "c="+c
			print cal
			global WIDTH
			WIDTH=int(cal[0])
			global HEIGHT
			HEIGHT=int(cal[1])
			global HMIN
			HMIN=int(cal[2])
			global HMAX
			HMAX=int(cal[3])
			global THRESHIGHLIGHT
			THRESHIGHLIGHT=int(cal[4])
			global THRES 
			THRES=int(cal[5])
			global LOWER_VIEW
			LOWER_VIEW = int(cal[6])
			global GAP
			GAP = WIDTH * (float(cal[7])/100)
			global ALFA
			ALFA=float(cal[8])
			#global ALFA_RADS
			#ALFA_RADS=float(1.0*ALFA/180*math.pi)
			global IMGSAVE
			IMGSAVE=int(cal[9])
			#global log
			#log.write(time.strftime("%d-%m-%Y, %H:%M:%S")+"Configuration: "+c)
			#global cap
			#cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,WIDTH)
			#cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,HEIGHT)
			break
'''

def normalize(arr):
	for i in range(3):
		minval = arr[...,i].min()
		maxval = arr[...,i].max()

	if minval != maxval:
		arr[...,i] -= minval
		arr[...,i] *= (255.0/(maxval-minval))
	return arr

def region(cx,cy):
	if ((cx == -1 or cy == -1) or (cx >= WIDTH - GAP - ((HEIGHT - cy)/math.tan(ALFA)))): 
		return 'R'
	if (cy > LOWER_VIEW and cx < WIDTH - GAP - ((HEIGHT - cy)/(math.tan(ALFA))) and cx > GAP + ((HEIGHT-cy)/(math.tan(ALFA))) ): 
		return 'S'
	if (cx <= GAP + ((HEIGHT - cy)/math.tan(ALFA))):
		return 'L'
	return 'F'

#log = open('cubielog.txt','a')
#log.write("\n\n\nStart at "+time.strftime("%d-%m-%Y, %H:%M:%S")+"\n\n")

cap=cv2.VideoCapture(-1)
cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,WIDTH)
cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,HEIGHT)
r,f = cap.read()

q=''
while 1:
	q = ser.read(1)
	if q == 'S':
		break
	
	#if q == 'C':
	#	print "q="+q
	#	settings()
	
	if q == 'Q':
		ret,frame = cap.read()
		if ret:
			cframe = frame[HMIN:HMAX, 1:WIDTH]
			gray = cv2.cvtColor(cframe, cv2.COLOR_BGR2GRAY)
			cframe = normalize(cframe) # check it when sunny	
			_,gray = cv2.threshold(gray,THRES,THRESHIGHLIGHT,0)

			#canny
			#edges = cv2.Canny(gray,100,200)
			#kernel = np.ones((5,5),np.float32)/10
			#edges = cv2.filter2D(edges,-1,kernel)
			
			# erode
			img_erode = gray
			kernel_erode = np.ones((2,2),np.uint8)
			erosion = cv2.erode(img_erode,kernel_erode,iterations = 5)

			#cnts,_ = cv2.findContours(edges.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
			cnts,_ = cv2.findContours(erosion.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
			cnts = sorted(cnts, key = cv2.contourArea, reverse = True)
			
			if len(cnts):
				cnt = cnts[0] #choose biggest contour
				M = cv2.moments(cnt)
				if M['m00']:
					cx = int(M['m10']/M['m00'])
					cy = int(M['m01']/M['m00'])
				#else:
					#log.write(time.strftime("%d-%m-%Y, %H:%M:%S")+"Error(1): cannot calculate centroid")
			else:
				# No white area found
				cx = -1
				cy = -1
			R = region(cx,cy)
			
			#log.write(time.strftime("%d-%m-%Y, %H:%M:%S")+"cx="+str(cx)+", cy="+str(cy)+", region="+str(R)+"\n")
			#if IMGSAVE==1:
				#cv2.imwrite("pic_"+str(time.time())+"_cframe.jpg",frame)
				#cv2.imwrite("pic_"+str(time.time())+"_erosion.jpg",erosion)
			ser.write(str(R)) #send anwser to arduino
			time.sleep(0.25)
		#else:
			#log.write(time.strftime("%d-%m-%Y, %H:%M:%S")+"Error(0): Problem with reading")
		q = ''

ser.close()
cap.release();

