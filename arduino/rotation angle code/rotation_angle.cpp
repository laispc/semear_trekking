//rotation angle function test
#include <iostream>
#include <cmath>
#include <string>

const double pi = 3.14159265358;

using namespace std;

/* EVERYTHING HERE IS SET ON DEFINITIONS */
float COMPASS_REFERENCE_ANGLE = 0; 		// Compass reference angle.
float currentAngle = 0; 				// Set in DEFINITIONS

//float latCurrentTarget = 0, lonCurrentTarget = 1;  
//float gpsCurrentLat = 0, gpsCurrentLon = 0;			// Where the robot is

float latCurrentTarget = -22.007480621, lonCurrentTarget = -47.897979736;  
float gpsCurrentLat = -22.007480621, gpsCurrentLon = -47.897979732;			// Where the robot is

float rotationAngle()
{
	// All angles in degrees
	float currentRobotAngle = currentAngle;
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

	cout << "target angle = " << targetAngle << endl ;

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

	return rotationAngle;
}

int main(int argc, char const *argv[])
{
	float g = rotationAngle();

	cout << "Rotation (degrees) = " << g << endl ;

	return 0;
}

