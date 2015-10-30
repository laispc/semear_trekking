#include <TinyGPS.h>


//_____________Global_variables_____________
int PointNum=0; //counter for check-points
float points[6] = {-22.0077991,-22.0077991,-22.0076694,-47.8974990,-47.8972206,-47.8972282}; //GPS data for check-points
float gpserror = 0.000045;
String cubieconf = "320,240,000,240,100,200,050,020,070,080,080,0,.";
float gpsCurrentLat, gpsCurrentLon;
float filterLat = -20;
float filterLon = -45;

float COMPASS_REFERENCE_ANGLE = 105.20;
int MOTORRIGHT1 = 6;
int MOTORRIGHT2 = 5;
int MOTORLEFT1 = 4;
int MOTORLEFT2 = 3;
int MAXIMUMMOTORPOWER = 50;
int DIFERENCEBETWEENMOTORS = 64;  //ddiferença máxima de potencia em sinal para locomoção
int ANGLETHRESHOLD = 60;     //angulo de erro em que o robô começa a girar em torno do proprio eixo na locomoção
float kp = 0.5;
int MAXINVERTPOWER = 32;

TinyGPS gps;
int smallangle = 200;
int POWERFORWARD = 32;

//________________Functions_________________
char askcubie(char key, String msg) {
	Serial2.write(key);
	
	int l = msg.length();
	if (l>0) {
		delay(300);
    int i;
    for (i=0;i<l;i++){
		  Serial2.write(msg[i]);
    }
	}
	if (key == 'Q'){
		while(1==1){
			if (Serial2.available()) {
				return Serial2.read();
			}
		}
	}
}

void getGPS(){
	bool newdata = false;
	unsigned long start = millis();
	while (millis() - start < 200) {    // msec to update gps data
		if (feedgps())  newdata = true;
	}
	if (newdata) {
		gpsdump(gps);
	}
}

void gpsdump(TinyGPS &gps) { 
	unsigned long age;
	gps.f_get_position(&gpsCurrentLat, &gpsCurrentLon, &age);
}

bool feedgps() {
	while (Serial1.available()) {
		if (gps.encode(Serial1.read()))
			return true;
	}
	return false;
}

void fire() {
	int i;
	for (i=0; i<=4; i++){
		digitalWrite (53,HIGH);
		delay(500);
		digitalWrite (53,LOW);
		delay(500);
	}
}

void calibration()
{
  float calib=millis();
  while(millis()-calib<5000)
    readIMU();
  digitalWrite (53,HIGH);
  delay(2000);
  digitalWrite (53,LOW);
}

float readIMU ()
{
float yaw;
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
  return yaw;
}

float rotationAngle(float lat, float lon, float yaw)
{
  // All angles in degrees
  float pi=3.14159265358979323846;
  float currentRobotAngle;
  float currentLat = lat; 
  float currentLon = lon;      			// Angle of target
  float rotationAngle;						// The answer we want!
  float targetAngle; 
   
  float dLon = points[PointNum+3] - lon;	// Analog to x coordinates
  float dLat = points[PointNum] - lat;	// Analog to y coordinates
    	
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
  
  rotationAngle=-targetAngle+currentRobotAngle;
  
    	// Rotation angles corrections    
  if (rotationAngle > 180)	rotationAngle = rotationAngle - 360;
  if (rotationAngle < -180)	rotationAngle = rotationAngle + 360;
    
  return rotationAngle;	// degrees
}

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

void vismove(char r) {
	// move by vision
	if (r=='R') {
	  turnClockwise();
	}
  if (r=='L') {
    turnCounterClockwise();
  }
  if (r=='F') {
    moveForward();
  }
  delay(smallangle);
  stopMotors();
}

void gpsmove(float lat, float lon) {
	// move by gps
	float yaw = readIMU();
	float angle = rotationAngle(lat,lon,yaw);
	moveUntilClose(angle);
}

//________________Main________________
void setup() {
	Serial.begin(115200);
	Serial2.begin(19200); //for vision
	Serial1.begin(9600); //for GPS
	Serial3.begin(57600); //for compass
	askcubie('C', cubieconf);
	pinMode(53,OUTPUT);
  stopMotors();
  calibration();
  
}

void loop() {
	while(1==1) 
	{ 
		//Serial.println("main loop");
		while (1==1)
		{
		  	getGPS();   
      		if ((gpsCurrentLat < filterLat) && (gpsCurrentLon<filterLon)) break;
		}
		float gpscurrent[2] = {gpsCurrentLat, gpsCurrentLon};
		
		float latcrit = (gpscurrent[0]-points[PointNum]);
    	float loncrit = (gpscurrent[1]-points[PointNum+3]);

    	if (latcrit<=0) latcrit = -1.0*latcrit;
    	if (loncrit<=0) loncrit = -1.0*loncrit;
		
		if (( latcrit < gpserror) && (loncrit < gpserror)) { 
//Serial.println("check-point");
			//motion by vision
      
			char region = 'S'; //= askcubie('Q',String(""));
			
			if (region == 'S'){
				stopMotorsPoint();
				fire();//Serial.println(PointNum);Serial.println(points[PointNum],7);
				PointNum++; 
				if (PointNum>2) { goto FINISH; }
			} else {
        vismove(region);
			}
		} else {
			//motion by gps
			gpsmove(gpscurrent[0],gpscurrent[1]);
		}
	}
	FINISH:
 Serial.println("finish");
	unsigned long t = millis();
	String ts="";
	int i;
	for (i=1;i<=(7-String(t).length());i++) ts+="0"; //timer always 7 digits
	ts+=String(t);
	ts+=",.";
	askcubie('S', "");
	while (1==1){delay(10000);}
}


