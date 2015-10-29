
void printTest ()
{
   getGPS(); // Get current GPS coordinates
    
    if (abs(latCurrentTarget-gpsCurrentLat)<0.000055 && abs(lonCurrentTarget-gpsCurrentLon)<0.000055)
    {
      Serial.println("moveToTarget()");
      Serial.println("Lampada vai piscar");
      Serial.println(gpsCurrentLat, 9);
      Serial.println(gpsCurrentLon, 9);
      delay(1000);
      lightBlink ();
    }
    else
    {
      yaw=readIMU();
      rotAngle = rotationAngle();
      Serial.println("moveUntilClose");
      Serial.println(gpsCurrentLat, 9);
      Serial.println(gpsCurrentLon, 9);
      Serial.println(rotAngle);
      Serial.println (targetAngle);
      Serial.println(lightCounter);
    }
}
