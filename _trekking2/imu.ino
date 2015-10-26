

float readIMU ()
{
  Serial3.flush();
  while(Serial3.available())
  { 
     leitura = Serial3.read();
   
     if (leitura!=',')
     {
       yawArray[i] = leitura;
       i+=1;
     }
     else
     {
       yawArray[i] = '\0';
       yaw = atof(yawArray);
       i=0;

     }
  }
  Serial.println(yaw);
  if(yaw < 0) yaw = yaw + 360;
  return yaw;
}
