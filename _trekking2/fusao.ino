// Fusão entre gps e imu

float rotationAngle()
{
  // All angles in degrees
  float pi=3.14159265358979323846;
  float currentRobotAngle;
  float currentLat = gpsCurrentLat; 
  float currentLon = gpsCurrentLon;      			// Angle of target
  float rotationAngle;						// The answer we want!
  float dLat, dLon;							// Diffences between robot and target coordinates
    
  dLon = lonCurrentTarget - gpsCurrentLon;	// Analog to x coordinates
  dLat = latCurrentTarget - gpsCurrentLat;	// Analog to y coordinates
    	
  if (dLon > -1E-10 and dLon < 1E-10 and dLat >= 0)	targetAngle = 90;
  else if (dLon > -1E-10 and dLon<1E-10 and dLat < 0)	targetAngle = -90;
  else	targetAngle = atan(dLat/dLon)*180/pi;
  if (dLon<0)
  {
    targetAngle+=180;
  }
  
  if(yaw>COMPASS_REFERENCE_ANGLE)
  {
    currentRobotAngle=COMPASS_REFERENCE_ANGLE-yaw+360;
  }
  else currentRobotAngle=COMPASS_REFERENCE_ANGLE-yaw;
  
  rotationAngle=-targetAngle+currentRobotAngle;
  
    	// Rotation angles corrections    
  if (rotationAngle > 180)	rotationAngle = rotationAngle - 360;
  if (rotationAngle < -180)	rotationAngle = rotationAngle + 360;
    
  return rotationAngle;	// degrees
}
