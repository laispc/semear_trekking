// Fusão entre gps e imu

float rotationAngle()
{
	// All angles in degrees
	float currentRobotAngle = yaw;
	float currentLat = gpsCurrentLat; 
	float currentLon = gpsCurrentLon;

	float targetAngle;							// Angle of target
	float rotationAngle;						// The answer we want!
	float dLat, dLon;							// Diffences between robot and target coordinates

	dLon = lonCurrentTarget - gpsCurrentLon;	// Analog to x coordinates
	dLat = latCurrentTarget - gpsCurrentLat;	// Analog to y coordinates
	
	if (dLon > -1E-10 and dLon < 1E-10 and dLat >= 0)	targetAngle = 90;
	else if (dLon > -1E-10 and dLon<1E-10 and dLat < 0)	targetAngle = -90;
	else	targetAngle = atan(dLat/dLon)*180/pi;

	if ((dLon >= 0 and dLat >= 0) or (dLon >= 0 and dLat <= 0))	// targetAngle in I or IV quads
	{
		rotationAngle = -(currentRobotAngle - COMPASS_REFERENCE_ANGLE - targetAngle);
	}
	else	// targetAngle in II or III quads
	{
		rotationAngle = -(currentRobotAngle - COMPASS_REFERENCE_ANGLE - targetAngle - 180);
	}

	// Rotation angles corrections

	if (rotationAngle > 180)	rotationAngle = rotationAngle - 360;
	if (rotationAngle < -180)	rotationAngle = rotationAngle + 360;

	return rotationAngle;	// degrees
}
