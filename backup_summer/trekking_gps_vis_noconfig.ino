#include <TinyGPS.h>




//_____________Global_variables_____________
int pointNum=0; //counter for check-points
float points[6] = {-21.2315667,-21.2317094,-21.2318191,-44.9914906,-44.9913044,-44.9914665}; //GPS data for check-points  , 

float targetAngle;

float gpserror = 0.000020;
float latcrit;
float loncrit;

//  Cubie Config order: WIDTH, HEIGHT, HMIN, HMAX, THRESHIGHLIGHT, THRES, LOWER_VIEW, GAP, ALFA, IMGSAVE
String cubieconf = "320,240,000,240,255,222,190,010,080,1,.";

float gpsCurrentLat, gpsCurrentLon;
float filterLat = -20;
float filterLon = -40;

float COMPASS_REFERENCE_ANGLE = 135.20;
int MOTORRIGHT1 = 6;
int MOTORRIGHT2 = 5;
int MOTORLEFT1 = 4;
int MOTORLEFT2 = 3;
int MAXIMUMMOTORPOWER = 32;
int DIFERENCEBETWEENMOTORS = 64;  	//diferença máxima de potencia em sinal para locomoção
int ANGLETHRESHOLD = 60;     		//angulo de erro em que o robô começa a girar em torno do proprio eixo na locomoção
float kp = 1.2;
int MAXINVERTPOWER = 20;
float yaw;
float imuAux;
TinyGPS gps;
int smallangle = 200;
int POWERFORWARD = 60;
int calibrationAux=0;
int YAWTHRESHOLD=60;
int DEBUG=1;     //1 to debug
float rotAngle;            // Debug rot angle
char region;

//________________Functions_________________

//Cubie_____________
char askcubie(char key, String msg) 
{
  Serial2.write(key);
	
  int l = msg.length();
  if (l>0) 
  {
  delay(300);
    int i;
    for (i=0;i<l;i++)
    {
      Serial2.write(msg[i]);
    }
  }
  if (key == 'Q')
  {
    while(1==1)
    {
      if (Serial2.available()) 
      {
	return Serial2.read();
        break;
      }
    }
  }
  Serial2.flush();
}

char readCubie()
{
  while(1==1)
  {
    if (Serial2.available()) 
    {
      return Serial2.read();
      break;
    }
  }
  //Serial2.flush();
}


//GPS_______________
void getGPS()
{
  bool newdata = false;
  unsigned long start = millis();
  while (millis() - start < 200) 
  {    // msec to update gps data
    if (feedgps())  newdata = true;
  }
  if (newdata) 
  {
    gpsdump(gps);
  }
}

void gpsdump(TinyGPS &gps) 
{ 
  unsigned long age;
  gps.f_get_position(&gpsCurrentLat, &gpsCurrentLon, &age);
}

bool feedgps() 
{
  while (Serial1.available()) 
  {
    if (gps.encode(Serial1.read()))
    return true;
  }
  return false;
}


void debug()
{
  Serial.println("DEBUG");
  Serial.println("");
  Serial.println("");
  if (( latcrit < gpserror) && (loncrit < gpserror))
  {
    Serial.println("vismove");
    Serial.print("Regiao=");
    Serial.println(region);
  }
  else Serial.println("moveUntilClose");

  Serial.print("yaw=");
  Serial.println (yaw);
  Serial.print("target angle=");
  Serial.println (targetAngle);
  Serial.print("rot angle=");
  Serial.println(rotAngle);
  Serial.print("current lat=");
  Serial.println(gpsCurrentLat, 9);
  Serial.print("current lon");
  Serial.println(gpsCurrentLon, 9);
  Serial.print("point num");
  Serial.println(pointNum);
  Serial.print("point lat");
  Serial.println(points[pointNum],9);
  Serial.print("point lon");
  Serial.println(points[pointNum+3],9);
}

//Blink___________
void fire() 
{
  int i;
  for (i=0; i<=4; i++)
  {
    digitalWrite (53,HIGH);
    delay(1000);
    digitalWrite (53,LOW);
    delay(1000);
   }
}

//Calibration________

void calibration()
{
  float calib=millis();
  while(millis()-calib<500)
  {
    readIMU();
  }
  calibrationAux=1;
  digitalWrite (53,HIGH);
  delay(500);
  digitalWrite (53,LOW);
  delay(500);
  while(calibrationAux==1)
  {  
    getGPS(); 
    if(gpsCurrentLat!=0 && gpsCurrentLon!=0) 
    {
      digitalWrite (53,HIGH);
      delay(500);
      digitalWrite (53,LOW);
      delay(500);
      calibrationAux=2;
    }
  }
}

//IMU________________

float readIMU ()
{

char imuRead;
char yawArray[15];

int i=0;		
  Serial3.flush();
  while(Serial3.available())
  { 
     imuRead = Serial3.read();
   
     if(imuRead!=',')
     {
       yawArray[i]=imuRead;
       i++;
     }
     else
     {
       yawArray[i] = '\0';
       yaw = atof(yawArray);
       i=0;
     }
  }
/*  if (calibrationAux!=0)
  {
    if (abs(imuAux-yaw)>YAWTHRESHOLD) yaw=imuAux;
  }
//  imuAux=yaw;    
*/
  return yaw;
}

//Fusion between IMU and GPS

float rotationAngle(float lati, float longi, float yaw)
{
  // All angles in degrees
  float pi=3.14159265358979323846;
  float currentRobotAngle;
  float currentLat = lati; 
  float currentLon = longi;      			// Angle of target
   
  float dLon = points[pointNum+3] - longi;	// Analog to x coordinates
  float dLat = points[pointNum] - lati;	// Analog to y coordinates
    	
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
  
  float rotationAngle=-targetAngle+currentRobotAngle;
  
    	// Rotation angles corrections    
  if (rotationAngle > 180)	rotationAngle = rotationAngle - 360;
  if (rotationAngle < -180)	rotationAngle = rotationAngle + 360;
    
  return rotationAngle;	// degrees
}

//Locomotion_________

void moveUntilClose(float angError)
{
float control=kp*angError;

  if (angError>ANGLETHRESHOLD||angError<=-ANGLETHRESHOLD)
  {
    if (angError>ANGLETHRESHOLD) turnClockwise();
    else turnCounterClockwise();
  }
  else
  {
    motorsControl(control);
  } 
} 

void turnCounterClockwise()
{
  analogWrite (MOTORRIGHT1, MAXINVERTPOWER);
  analogWrite (MOTORRIGHT2, MAXINVERTPOWER);
  analogWrite (MOTORLEFT1, 255-MAXINVERTPOWER);
  analogWrite (MOTORLEFT2, 255-MAXINVERTPOWER);
}

void turnClockwise()
{
  analogWrite (MOTORRIGHT1, 255-MAXINVERTPOWER);
  analogWrite (MOTORRIGHT2, 255-MAXINVERTPOWER);
  analogWrite (MOTORLEFT1, MAXINVERTPOWER);
  analogWrite (MOTORLEFT2, MAXINVERTPOWER);
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
  if (powerToMotor<=0)
  {
    analogWrite (MOTORRIGHT1, MAXIMUMMOTORPOWER);
    analogWrite (MOTORRIGHT2, MAXIMUMMOTORPOWER);
    analogWrite (MOTORLEFT1, MAXIMUMMOTORPOWER-powerToMotor);
    analogWrite (MOTORLEFT2, MAXIMUMMOTORPOWER-powerToMotor);
  }
  else
  {
    analogWrite (MOTORRIGHT1, MAXIMUMMOTORPOWER+powerToMotor);
    analogWrite (MOTORRIGHT2, MAXIMUMMOTORPOWER+powerToMotor);
    analogWrite (MOTORLEFT1, MAXIMUMMOTORPOWER);
    analogWrite (MOTORLEFT2, MAXIMUMMOTORPOWER);
  }
}

void stopMotors()
{
  analogWrite (MOTORRIGHT1, 128);
  analogWrite (MOTORRIGHT2, 128);
  analogWrite (MOTORLEFT1, 128);
  analogWrite (MOTORLEFT2, 128);
}

void stopMotorsPoint()
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

//General functions________________
void vismove(char r) 
{
  // move by vision
  if(r=='R' || r=='L')
  {
    if (r=='R') 
    {
      turnClockwise();
    }
    if (r=='L')
    {
      turnCounterClockwise();
    }
    delay(smallangle);
    stopMotors();
  }
  if (r=='F') 
  {
    moveForward();
  }
  
}

void gpsmove(float lat, float lon) 
{
  // move by gps
  float yaw = readIMU();
  float angle = rotationAngle(lat,lon,yaw);
  rotAngle = angle;
  moveUntilClose(angle);
}


//________________Main________________
void setup() 
{
  

  Serial.begin(115200);
  Serial2.begin(19200); //for vision
  Serial1.begin(9600); //for GPS
  Serial3.begin(57600); //for compass
  
  //askcubie('C', cubieconf);
  
  pinMode(53,OUTPUT);
  stopMotors();
  
  //calibration();
}

void loop() 
{
  while(1==1) 
  {
    unsigned long startTime = millis(); // Debug loop time
    unsigned long endTime;
    
    //Serial.println("main loop");
    while (1==1)
    {
      getGPS();   
      if ((gpsCurrentLat < filterLat) && (gpsCurrentLon<filterLon)) break;
    }
    float gpscurrent[2] = {gpsCurrentLat, gpsCurrentLon};

    latcrit = (gpscurrent[0]-points[pointNum]);
    loncrit = (gpscurrent[1]-points[pointNum+3]);

    if (latcrit<=0) latcrit = -1.0*latcrit;
    if (loncrit<=0) loncrit = -1.0*loncrit;
		
    if (( latcrit < gpserror) && (loncrit < gpserror))
      {  
        //Serial.println("check-point");
        //motion by vision
  /*      stopMotors();
        delay (200);
        turnClockwise();
        delay(200);
        moveForward();
        delay(2000);
        fire();*/
        
       //region = askcubie('Q',String(""));  			
       region = readCubie();
      if (region == 'S')
      { 
  //     moveForward();
  //     delay(500);
         stopMotors();
  	     fire();//Serial.println();Serial.println(points[],7);
  	     pointNum++;
  	     if (pointNum>2) { goto FINISH; }
      } 
      else 
      {
         // motion by camera
       	vismove(region);
      }
    }
 
    else 
    {
    //motion by gps
      gpsmove(gpscurrent[0],gpscurrent[1]);
    }
    
    if (DEBUG==1)
    {
      debug();
    }
  }

    //endTime = millis();
    //Serial.print("Total time=");
     //Serial.println(endTime - startTime);
}
//  FINISH:
//  Serial.println("finish");
//  unsigned long t = millis();
//  String ts="";
//  int i;
//  for (i = 1;i <= (7-String(t).length());i++) ts+="0"; //timer always 7 digits
//  ts+=String(t);
//  ts+=",.";
//  askcubie('S', "");
//  while (1 == 1){delay(10000);}

