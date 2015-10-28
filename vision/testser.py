#!/usr/bin/python

import serial
import time

B=9600
ser = serial.Serial('/dev/ttyS0', B, timeout=0)

#ser.open();
ser.flushInput()


while 1:

	q = ser.read(1)
	print q	
	time.sleep(0.1)
	if (q == "Q"):
		ser.write("F")

	
	
ser.close()
