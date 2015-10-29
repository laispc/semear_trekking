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
import os

# Define constants
theta = 58
H = 0.4
alpha = 80
npixel = 480

# Define view areas
WIDTH = 640
HEIGHT = 480
LOWER_VIEW = HEIGHT - 0.3*HEIGHT
GAP = 0.15*WIDTH
ALFA = 80
ALFA_RADS = float(1.0*ALFA/180*math.pi)	#degrees

CENTER_VIEW = WIDTH/2
LEFT_VIEW = CENTER_VIEW - 80
RIGHT_VIEW = CENTER_VIEW + 80 

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
	
def region(cx,cy):
	if ((cx == -1 or cy == -1) or (cx >= WIDTH - GAP - (cy/math.tan(ALFA)))): 
		return "R"
	if (cy > LOWER_VIEW and cx < WIDTH - GAP - (cy/(math.tan(ALFA))) and cx > GAP + (cy/(math.tan(ALFA))) ): 
		return "S"
	if (cx <= GAP + (cy/math.tan(ALFA))):
		return "L"
	return "F"


if __name__ == "__main__":

	# Treat input argments

	if (len(sys.argv) < 4):
		print "Missing argument."
		print "Argv[1]: 'cam' (load camera) or 'file' (load from file)."
		print "Argv[2]: type camera index or file path."
		print "Argv[3]: thmax."
		sys.exit(0)

	arg_option = sys.argv[1]
	arg_address = sys.argv[2]
	arg_thmax = sys.argv[3]

	if (arg_option == 'cam'):

		fourcc = cv2.cv.CV_FOURCC(*'YUYV')
		#out = cv2.VideoWriter('out_cam' + arg_address + '.avi',fourcc, 10, (640,npixel)) # w,h
		cap = cv2.VideoCapture(int(arg_address))
		name_window = 'out_cam' + str(arg_address)
		print "Selected camera index:" + repr(arg_address)

	elif (arg_option == 'file'):

		cap = cv2.VideoCapture(arg_address)
		name_window = 'File: ' + str(arg_address)
		print "Loading video from file:" + repr(arg_address)

	else:
		print "Unknown option."
		sys.exit(0)

	#try to fix the bug in uvc driver
	cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,WIDTH)
	cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,HEIGHT)
	print "test"
	ret, frame = cap.read() #ret is a bool

	# Calibration constants!
	hmin = 0 	# Start of height
	hmax = WIDTH	# End of height
	#thmax = 223	# Minimum value for the area to be WHITE
	thmax = float(arg_thmax)
	thmin = 255	# Final color of areas classified as WHITE

	#Create trackbars in "Control" window
	'''
	cv2.namedWindow('Control', cv2.CV_WINDOW_AUTOSIZE)
	cv2.createTrackbar('hmax', 'Control', hmax, HEIGHT, nothing) # On action parameter: nothing()
	cv2.createTrackbar('hmin', 'Control', hmin, HEIGHT, nothing)
	cv2.createTrackbar('thmax', 'Control', thmax, 255, nothing)
	cv2.createTrackbar('thmin', 'Control', thmin, 255, nothing)	
	'''
	rcv = "";
	#ser = serial.Serial('/dev/ttyS0', 115200)
	#ser.flushInput()

	print "Start."
	k = ord('a')
	keep_going = False

	print "cam open = " + str(cap.isOpened())

	while(k != ord('q')):
		
		#time.sleep(0.5)
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
		'''
		ret, frame = cap.read()
		cframe = frame[hmin:hmax, 1:WIDTH]
		
		cframe = normalize(cframe) # check it when sunny

		if ret==True:
			gray = cv2.cvtColor(cframe, cv2.COLOR_BGR2GRAY)
			_,gray = cv2.threshold(gray,thmax,thmin,0)
			
			


			edges = cv2.Canny(gray,100,200)
			kernel = np.ones((5,5),np.float32)/10
			edges = cv2.filter2D(edges,-1,kernel)

			#erode
			img_erode = gray
			kernel_erode = np.ones((4,4),np.uint8)
			erosion = cv2.erode(img_erode,kernel_erode,iterations = 3)
			cv2.imshow('erode',erosion)
			
			#(cnts, _) = cv2.findContours(edges.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
			(cnts, _) = cv2.findContours(erosion.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
			cnts = sorted(cnts, key = cv2.contourArea, reverse = True)
			
			if len(cnts):
				cnt = cnts[0]

				M = cv2.moments(cnt)
				if M['m00']:
					cx = int(M['m10']/M['m00'])
					cy = int(M['m01']/M['m00'])

					#d = distance(0.4, 36, 50, 480, cy)
					#print "distance:" + repr(d)
			
					cv2.circle(erosion,(cx,cy),10,0,-1)
					cv2.circle(edges,(cx,cy),10,128,-1)
					
					#cv2.imshow(name_window,frame)
					cv2.imshow('Filter',gray)
					#if (arg_option == 'cam'):
						#out.write(frame)
				else:
					print "Error: Can't calculate centroid"
			else:
				cx = -1
				cy = -1
				#print "Warning: No white areas.\n"

			print region(cx, cy) + " " + str(cx) + " " + str(cy)

			if cv2.waitKey(100) & 0xFF == ord('q'):
				break
		else:
			print "Error: Problem with cam."
			break

	cap.release()
	out.release()
	cv2.destroyAllWindows()

	sys.exit(0)
