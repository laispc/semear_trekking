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

# Define view areas
WIDTH = 640/2
HEIGHT = 480/2

GAP = 0.15*WIDTH
ALFA = 80	#degrees
ALFA_RADS = float(1.0*ALFA/180*math.pi)	#radians

CENTER_VIEW = WIDTH/2
LEFT_VIEW = CENTER_VIEW - 80
RIGHT_VIEW = CENTER_VIEW + 80 
LOWER_VIEW = HEIGHT*(1 - 0.3)

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
	if ((cx == -1 or cy == -1) or (cx >= WIDTH - GAP - ((HEIGHT - cy)/math.tan(ALFA)))): 
		return "R"
	if (cy > LOWER_VIEW and cx < WIDTH - GAP - ((HEIGHT - cy)/(math.tan(ALFA))) and cx > GAP + ((HEIGHT-cy)/(math.tan(ALFA))) ): 
		return "S"
	if (cx <= GAP + ((HEIGHT - cy)/math.tan(ALFA))):
		return "L"
	return "F"


if __name__ == "__main__":

	# Treat input argments

	if (len(sys.argv) < 4):
		print "Missing argument."
		print "Argv[1]: 'cam' (load camera) or 'file' (load from file)."
		print "Argv[2]: type camera index or file path."
		print "Argv[3]: thres."
		sys.exit(0)

	arg_option = sys.argv[1]
	arg_address = sys.argv[2]
	arg_thres = sys.argv[3]

	if (arg_option == 'cam'):

		#fourcc = cv2.cv.CV_FOURCC(*'YUYV')
		#out = cv2.VideoWriter('out_cam' + arg_address + '.avi',fourcc, 10, (WIDTH, HEIGHT)) # w,h
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
	print "Vision test"
	ret, frame = cap.read() #ret is a bool

	# Calibration constants!
	hmin = 0 		# Start of height
	hmax = HEIGHT	# End of height
	#thres = 223	# Minimum value for the area to be WHITE
	thres = float(arg_thres)
	thres_highlight = 255	# Final color of areas classified as WHITE

	print "Camera is opended = " + str(cap.isOpened())

	while(cv2.waitKey(100) != ord('q')):
		
		#time.sleep(0.25)

		ret, frame = cap.read()
		cframe = frame[hmin:hmax, 1:WIDTH]
		
		cframe = normalize(cframe) # check it when sunny

		if ret==True:
			gray = cv2.cvtColor(cframe, cv2.COLOR_BGR2GRAY)
			_,gray = cv2.threshold(gray,thres,thres_highlight,0)
			
			# canny
			#edges = cv2.Canny(gray,100,200)
			#kernel = np.ones((5,5),np.float32)/10
			#edges = cv2.filter2D(edges,-1,kernel)

			# erode
			img_erode = gray
			kernel_erode = np.ones((2,2),np.uint8)
			erosion = cv2.erode(img_erode,kernel_erode,iterations = 5)
			
			#(cnts, _) = cv2.findContours(edges.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
			(cnts, _) = cv2.findContours(erosion.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
			cnts = sorted(cnts, key = cv2.contourArea, reverse = True)
			
			if len(cnts):
				cnt = cnts[0]

				M = cv2.moments(cnt)
				if M['m00']:
					cx = int(M['m10']/M['m00'])
					cy = int(M['m01']/M['m00'])
			
					cv2.circle(erosion,(cx,cy),10,0,-1)
					#cv2.circle(edges,(cx,cy),10,128,-1)
					
					# Show results
					cv2.imshow(name_window,frame)
					cv2.imshow('Eroded',erosion)
					#cv2.imshow('Filter',gray)
					
					#if (arg_option == 'cam'):
						#out.write(frame)
				else:
					print "Error: Can't calculate centroid"
			else:
				#No white areas!
				cx = -1
				cy = -1

			print region(cx, cy) + " " + str(cx) + " " + str(cy)

			if cv2.waitKey(100) & 0xFF == ord('q'):
				break
		else:
			print "Error: Problem with cam."
			break

	cap.release()
	#out.release()
	cv2.destroyAllWindows()

	sys.exit(0)
