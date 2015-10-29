void calibration()
{
  float calib=millis();
  while(millis()-calib<5000)
  {
    readIMU();
  }
  digitalWrite (LAMPADA,HIGH);
  delay(2000);
  digitalWrite (LAMPADA,LOW);
  delay(500);  

}

