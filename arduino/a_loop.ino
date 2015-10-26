using namespace TinyGPS;

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
    calibracao();
    break;
    
    case 2:
    tempoInicial = millis();
    getGPS(); // Get current GPS coordinates
    distanceToTarget = distance_between (gpsCurrentLat, gpsCurrentLon, latCurrentTarget, lonCurrentTarget); 
    
    if (distanceToTarget < DISTANCETHRESHOLD)
    {
      moveToTarget();
    }
    else
    {
      moveUntilClose();
    }

    break;
    
    
  }// end switch

}//end main loop


