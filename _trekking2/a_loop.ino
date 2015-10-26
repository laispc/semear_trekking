//using namespace TinyGPS;

void loop()
{ 
  switch (state)
  {
    case 0:
    if (DEBUG)
    {
      printTest();
    }
    else  state++;
    break;
    
    case 1:
    calibration();
    state++;
    break;
    
    case 2:
    initialLoopTime = millis();
    getGPS(); // Get current GPS coordinates
    
    if (abs(latCurrentTarget-gpsCurrentLat)<0.000055 && abs(lonCurrentTarget-gpsCurrentLon)<0.000055)
    {
      moveToTarget();
    }
    else
    {
      yaw=readIMU();
      rotAngle = rotationAngle();
      moveUntilClose(rotAngle);
    }

    break;
    
    
  }// end switch

}//end main loop
