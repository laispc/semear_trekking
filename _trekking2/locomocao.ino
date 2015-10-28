void moveUntilClose(float angError)
{
  control=kp*angError;
  if (angError>ANGLETHRESHOLD||angError<=-ANGLETHRESHOLD)
  {
    if (angError>ANGLETHRESHOLD) turnCounterClockwise();
    else turnClockwise();
//    currentLoopTime=millis();
  }
  else
  {
//    currentLoopTime=millis();
    motorsControl(control);
  } 
} 


void moveToTarget()
{
  numberCubie=readCubie();
  switch (numberCubie)
  {
  case 0:
  
  stopMotors();
  delay(500);
  lightBlink();
  control=0;
//  sumError=0;
  break;
  
  case 1:
  
  turnClockwise();
  break;
  
  case 2:
  
  turnCounterClockwise();
  break;
  
  case 3:
  
  moveForward();
  break;
  }
} 


void stopMotors()
{
  for (int i=0; i<=128; i++)
  {
  analogWrite (MOTORRIGHT1, i);
  analogWrite (MOTORRIGHT2, i);
  analogWrite (MOTORLEFT1, i);
  analogWrite (MOTORLEFT2, i);
  delay(5);
  }
}

void moveForward()
{
  analogWrite (MOTORRIGHT1, POWERFORWARD);
  analogWrite (MOTORRIGHT2, POWERFORWARD);
  analogWrite (MOTORLEFT1, POWERFORWARD);
  analogWrite (MOTORLEFT2, POWERFORWARD);
}

void turnCounterClockwise()
{
  analogWrite (MOTORRIGHT1, MAXIMUMMOTORPOWER);
  analogWrite (MOTORRIGHT2, MAXIMUMMOTORPOWER);
  analogWrite (MOTORLEFT1, 255-MAXIMUMMOTORPOWER);
  analogWrite (MOTORLEFT2, 255-MAXIMUMMOTORPOWER);
}

void turnClockwise()
{
  analogWrite (MOTORRIGHT1, 255-MAXIMUMMOTORPOWER);
  analogWrite (MOTORRIGHT2, 255-MAXIMUMMOTORPOWER);
  analogWrite (MOTORLEFT1, MAXIMUMMOTORPOWER);
  analogWrite (MOTORLEFT2, MAXIMUMMOTORPOWER);
}

void motorsControl(float controlMotor)
{
  float powerToMotor; //variavel auxiliar para ser inserida no controle dos motores
  if (controlMotor>DIFERENCEBETWEENMOTORS)
  {
    powerToMotor=DIFERENCEBETWEENMOTORS;
  }
  if(controlMotor<-DIFERENCEBETWEENMOTORS)
  {
    powerToMotor=-DIFERENCEBETWEENMOTORS;
  }
  else powerToMotor=controlMotor;
  if (powerToMotor>=0)
  {
    analogWrite (MOTORRIGHT1, MAXIMUMMOTORPOWER);
    analogWrite (MOTORRIGHT2, MAXIMUMMOTORPOWER);
    analogWrite (MOTORLEFT1, MAXIMUMMOTORPOWER+powerToMotor);
    analogWrite (MOTORLEFT2, MAXIMUMMOTORPOWER+powerToMotor);
//    Serial.println("Esquerda");
//    Serial.flush();
  }
  else
  {
    
    analogWrite (MOTORRIGHT1, MAXIMUMMOTORPOWER-powerToMotor);
    analogWrite (MOTORRIGHT2, MAXIMUMMOTORPOWER-powerToMotor);
    analogWrite (MOTORLEFT1, MAXIMUMMOTORPOWER);
    analogWrite (MOTORLEFT2, MAXIMUMMOTORPOWER);
//    Serial.println("Direita");
//    Serial.flush();
  }
}

