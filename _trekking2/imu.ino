float readIMU ()
{
  Serial3.flush();
  while(Serial3.available())
  { 
     imuRead = Serial3.read();
   
     if(imuRead!=',')
     {
       yawArray[i]=imuRead;
       i+=1;
     }
     else
     {
       yawArray[i] = '\0';
       yaw = atof(yawArray);
       i=0;
     }
  }
//  Serial.println(yaw);
  if (yaw<0) yaw2=yaw+360;
  else yaw2=yaw;
  return yaw2;
}
