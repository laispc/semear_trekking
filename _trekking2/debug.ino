
void printTest ()
{
   getGPS(); // Get current GPS coordinates
    
    if (abs(latCurrentTarget-gpsCurrentLat)<0.000055 && abs(lonCurrentTarget-gpsCurrentLon)<0.000055)
    {
      Serial.println("moveToTarget()");
      Serial.println("Lampada vai piscar");
      Serial.println(gpsCurrentLat, 7);
      Serial.println(gpsCurrentLon, 7);
      delay(1000);
      lightBlink ();
    }
    else
    {
      yaw=readIMU();
      rotAngle = rotationAngle();
      Serial.println("moveUntilClose");
      Serial.println(gpsCurrentLat);
      Serial.println(gpsCurrentLon);
      Serial.println(yaw);
      Serial.println(yaw2);
      Serial.println(lightCounter);
    }
}
