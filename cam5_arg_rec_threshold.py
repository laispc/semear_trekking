# Test version
# No serial writing
#
# Usage: 	argument argv[1] can be 'cam' (load camera) OR 'file' (load from file)
#			argument argv[2] camera index OR file path
# Controls: c - continuos mode
#			p - frame-by-frame mode
#			q - quit
#
# For argv[2] = 'file', file video capture is saved as 'output'+cam_number+'.avi'
# Threshold is applied and showed on screen

#!/usr/bin/python

import sys
import numpy as np
import cv2
import math
import serial
import time
import os, pygame
from pygame.locals import *

# Define constants
theta = 58
H = 0.4
alpha = 36
npixel = 480

'''
def distance(H, alpha, theta, npixel, y):
	d1 = H/math.cos(math.radians(theta))
	z = math.sin(math.radians(alpha/2))*H*2/math.cos(math.radians(theta))
	dpx = y*z/npixel
	omega = 90 + alpha/2
	g = math.sin(math.radians(omega))**2
	#print g
	r = (math.cos(math.radians(omega)) + (d1/dpx))**2 + math.sin(math.radians(omega))**2
	phi = math.degrees(math.asin(math.sqrt(g/r)))
	epsilon = 90 + alpha/2 - phi
	gama = epsilon - theta - alpha/2
	d = math.sin(math.radians(180 - epsilon))*dpx/math.sin(math.radians(gama))
	distancia = d + H*math.tan(math.radians(theta));
	return distancia
'''
def nothing(x):
	pass

def normalize(arr):
	for i in range(3):
		minval = arr[...,i].min()
		maxval = arr[...,i].max()

	if minval != maxval:
		arr[...,i] -= minval
		arr[...,i] *= (255.0/(maxval-minval))
	return arr

def navigation(cx,cy,pitch):
	#print cx, cy # for a while for tests
	print cx
	'''
	if len(pitch) > 0:
		#d = distance(H, alpha, theta+pitch, npixel, cy)
		ser.write(d)
		ser.write(";")
		ser.write(cx)
		ser.write('.')
	'''

	return 0


if __name__ == "__main__":

	# Treat input argments

	if (len(sys.argv) < 3):
		print "Missing argument."
		print "Argv[1]: 'cam' (load camera) or 'file' (load from file)."
		print "Argv[2]: type camera index or file path."
		sys.exit(0)

	arg_option = sys.argv[1]
	arg_address = sys.argv[2]

	if (arg_option == 'cam'):

		fourcc = cv2.cv.CV_FOURCC(*'YUYV')
		out = cv2.VideoWriter('output'+arg_address+'.avi',fourcc, 10, (640,npixel)) # w,h
		cap = cv2.VideoCapture(int(arg_address))
		print "Selected camera index:" + repr(arg_address)

	elif (arg_option == 'file'):

		cap = cv2.VideoCapture(arg_address)
		print "Loading video from file:" + repr(arg_address)

	else:
		print "Unknown option."
		sys.exit(0)

	#try to fix the bug in uvc driver
	cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,1280)
	cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,720)

	ret, frame = cap.read() #ret is a bool

	# Calibration constants!
	hmin = 210 	# Start of height
	hmax = 480	# End of height
	thmax = 162	# Minimum value for the area to be WHITE
	thmin = 255	# Final color of areas classified as WHITE

	#Create trackbars in "Control" window
	cv2.namedWindow('Control', cv2.CV_WINDOW_AUTOSIZE)
	cv2.createTrackbar('hmax', 'Control', hmax, 480, nothing) # On action parameter: nothing()
	cv2.createTrackbar('hmin', 'Control', hmin, 480, nothing)
	cv2.createTrackbar('thmax', 'Control', thmax, 255, nothing)
	cv2.createTrackbar('thmin', 'Control', thmin, 255, nothing)	
	rcv = "";
	#ser = serial.Serial('/dev/ttyS0', 115200)
	#ser.flushInput()

	print "start"
	k = ord('a')
	keep_going = False

	print "cam open = " + str(cap.isOpened())

	while(k != ord('q')):

		k = ord('a')

		# Treat keyboard input
		if (keep_going == True):
			k = cv2.waitKey(100);
			if k == ord('c'):
				keep_going = not keep_going
				print 'Frame by frame'

		else:
			while (k != ord('p') and k != ord('q') and k != ord('c')):
				k = cv2.waitKey()
				print 'Next'
				if k == ord('c'):
					keep_going = not keep_going
					print 'Continue'

		# in first, we check incoming message from Arduino
		'''
		val = ser.read()
		while len(val) > 0:
			if val == '.':
				arduino = rcv
				rcv = ""
				break
			else:
				rcv += val
				val = ser.read()
		'''
		#update and print calibration constants
		hmax = cv2.getTrackbarPos('hmax', 'Control')
		#print "hmax:", hmax
		hmin = cv2.getTrackbarPos('hmin', 'Control')
		#print "hmin:" + repr(hmin)
		thmax = cv2.getTrackbarPos('thmax', 'Control')
		#print "thmax:" + repr(thmax)
		thmin = cv2.getTrackbarPos('thmin', 'Control')
		#print "thmin:" + repr(thmin)

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
			cnts = sorted(cnts, key = cv2.contourArea, reverse = True)
			
			if len(cnts):
				cnt = cnts[0]

				M = cv2.moments(cnt)
				if M['m00']:
					cx = int(M['m10']/M['m00'])
					cy = int(M['m01']/M['m00'])

					#d = distance(0.4, 36, 50, 480, cy)
					#print "distance:" + repr(d)
			
					cv2.circle(gray,(cx,cy),10,0,-1)
					cv2.circle(edges,(cx,cy),10,128,-1)
					if navigation(cx,cy,"arduino") == -1:
						print "Error: Can't send data to arduino"
				
					cv2.imshow('2',frame)
					cv2.imshow('1',gray)
					#out.write(frame)
				else:
					print "Error: Can't calculate centroid"
			else:
				print "Warning: No white areas"
			if cv2.waitKey(100) & 0xFF == ord('q'):
				break
		else:
			print "Error: Problem with a cam"
			break

	cap.release()
	#out.release()
	cv2.destroyAllWindows()

	sys.exit(0)