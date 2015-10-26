#!/usr/bin/python
# madrugadadesexta.py
import numpy as np
import cv2
import math
import serial
import time

def normalize(arr):
	for i in range(3):
		minval = arr[...,i].min()
		maxval = arr[...,i].max()

	if minval != maxval:
		arr[...,i] -= minval
		arr[...,i] *= (255.0/(maxval-minval))
	return arr

def navigation(cx,cy,log):
	log.write("cx="+str(cx)+" cy="+str(cy)+" arduino says: "+question+time.strftime("%H:%M:%S")+"\n")
	ser.write(str(cx))
	ser.write(';')
	ser.write(str(cy))
	ser.write('.')
	return 0
cap = cv2.VideoCapture(0)

#cap.set(cv2.cv.CV_CAP_PROP_FPS,30)
cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,640)
cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,480)


ret, frame = cap.read()

fourcc = cv2.cv.CV_FOURCC(*'XVID')
out = cv2.VideoWriter('output.avi',fourcc, 10, (640,480)) # w,h

# Calibration constants!
hmin = 0 # it means horizont: 5 meters of distance. need in calibration.
hmax = 480 # 0-point: distance = 0 meters
thmax = 225
thmin = 115

rcv = ""
ser = serial.Serial('/dev/ttyS0', 115200)
ser.flushInput()
log = open('camlog.txt','a')
log.write("\n\n\nStart at "+time.strftime("%d-%m-%Y, %H:%M:%S")+"\n\n")
while(cap.isOpened()):	
	# in first, we check incoming message from Arduino
	'''	
	btr = ser.inWaiting()
	if btr > 0:
		rcv = ser.read(btr)
	else:
		rcv = ""
		#if rcv[-1] == '.':
		#	ard = rcv

	'''
	ret, frame = cap.read()
	cframe = frame[hmin:hmax, 1:640]
	
	#cframe = normalize(cframe) # check it when sunny
	
	if ret==True:
		gray = cv2.cvtColor(cframe, cv2.COLOR_BGR2GRAY)
		_,gray = cv2.threshold(gray,thmax,thmin,0)
		
		
		edges = cv2.Canny(gray,100,200)
		kernel = np.ones((5,5),np.float32)/10
		edges = cv2.filter2D(edges,-1,kernel)
		(cnts, _) = cv2.findContours(edges.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
		#cnts = sorted(cnts, key = cv2.contourArea, reverse = True)
		myaux=1
		mx=-1
		my=-1
		for i in range(0 ,len(cnts)):
			mom=cv2.moments(cnts[i])
			myaux=int(mom['m01']/mom['m00'])
			mxaux=int(mom['m10']/mom['m00'])
			area=cv2.contourArea(cnts[i])
			
			if myaux>my and area>1300: 
				my=myaux
				areaaux=area
				mx=mxaux
		out.write(frame)
		#log.write("cx="+str(mx)+" cy="+str(my)+" arduino says: "+question+time.strftime("%H:%M:%S")+"\n")
		ser.write('1')
		ser.write(str(mx))
		ser.write(';')
		ser.write(str(my))
		ser.write('.')
		
		if cv2.waitKey(100) & 0xFF == ord('q'):
			break
	else:
		log.write("Error: Problem with a cam\n")
		break
close(log)
cap.release()
out.release()

