void lerIMU ()
{
  //inicio leitura IMU
  Serial3.flush();// inicia a serial apenas quando for ler
  while(Serial3.available())
  {
    //Serial.println("while imu");
    c = Serial3.read(); // le "bit a bit" -0.38,:-33.56;-0.39,:-33.55;-0.38,:-33.54;-0.40,:
    if(aux==2){
      if(c!=','){
        pitchArray[i]= c; //string recebe c
        i+=1;
      } 
      if(c==','){
        pitchArray[i] = '\0';
        pitch = atof(pitchArray);
        i = 0;
        aux=0;
      }
    }
    if (aux==1){
      if(c!=';'){
        yaw[i]= c; //string recebe c
        i+=1;
      } 
      if(c==';'){
        yaw[i] = '\0';
        angMeas = atof(yaw);
        i = 0;
        aux=2;
      }
    }
    if (c==':'){
      aux=1; // espera aparecer ':' para o inicio da leitura.
    }
  }
  aux=0;
}

