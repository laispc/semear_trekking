############### DESCRIPTION ###############
#Save a video record to use without robot #
#                                         #
# BY Hugo                                 #
#                                         #
############### DESCRIPTION ###############
import cv2
import numpy as np
import sys
from time import sleep

#Receive and count quantity of arguments
argv = sys.argv[0:];
argc = len(argv);

#camera argument - char '0' is 48 in ASCII
camera = ord(argv[1])-48;
#video filename
videofile = argv[2]

if (argc < 2):
    print '\nMissing arguments. Need 3 but has', argc+1
    print 'Try "python teste.py <camera> <videofile>"\n'
    quit()

cap = cv2.VideoCapture(camera)

# Define the codec and create VideoWriter object
fourcc = cv2.cv.CV_FOURCC('X','V', 'I', 'D')
out = cv2.VideoWriter(videofile,fourcc, 20.0, (640,480))

while(cap.isOpened()):
    sleep(0.5)
    ret, frame = cap.read()
    if ret==True:

        # write the flipped frame
        out.write(frame)

        cv2.imshow('frame',frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    else:
        break

# Release everything if job is finished
cap.release()
out.release()
cv2.destroyAllWindows()