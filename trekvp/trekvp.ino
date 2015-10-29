#include <TinyGPS.h>


//_____________Global_variables_____________
int i=0; //counter for check-points
float points[6] = {1,2,3,1,2,3}; //GPS data for check-points
String cubieconf = "320,240,000,240,100,200,050,020,070,080,080,0,.";
float gpsCurrentLat, gpsCurrentLon;
float gpserror = 0.00001;
float COMPASS_REFERENCE_ANGLE = 105.20;
int MOTORRIGHT1 = 6;
int MOTORRIGHT2 = 5;
int MOTORLEFT1 = 4;
int MOTORLEFT2 = 3;
int MAXIMUMMOTORPOWER = 32;
int DIFERENCEBETWEENMOTORS = 64;  //ddiferença máxima de potencia em sinal para locomoção
int ANGLETHRESHOLD = 50;     //angulo de erro em que o robô começa a girar em torno do proprio eixo na locomoção


//________________Functions_________________
char askcubie(char key, String msg) {
	Serial2.write(key);
	
	int l = msg.length();
	if (l>0) {
		delay(200);
		Serial2.write(msg);
	}
	if (key == "Q"){
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
	while (Serial3.available()) {
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
  digitalWrite (LAMPADA,HIGH);
  delay(2000);
  digitalWrite (LAMPADA,LOW);
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
   
  float dLon = points[i+3] - lon;	// Analog to x coordinates
  float dLat = points[i] - lat;	// Analog to y coordinates
    	
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
float kp = DIFERENCEBETWEENMOTORS/ANGLETHRESHOLD;
float control=kp*angError;

  if (angError>ANGLETHRESHOLD||angError<=-ANGLETHRESHOLD)
  {
    if (angError>ANGLETHRESHOLD) turnCounterClockwise();
    else turnClockwise();
  }
  else
  {
    motorsControl(control);
  } 
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
  }
  else
  {
    
    analogWrite (MOTORRIGHT1, MAXIMUMMOTORPOWER-powerToMotor);
    analogWrite (MOTORRIGHT2, MAXIMUMMOTORPOWER-powerToMotor);
    analogWrite (MOTORLEFT1, MAXIMUMMOTORPOWER);
    analogWrite (MOTORLEFT2, MAXIMUMMOTORPOWER);
  }
}

void vismove(char r) {
	// move by vision
	
	
}

void gpsmove(float lat, float lon) {
	// move by gps
	float yaw = readIMU();
	float angle = rotationAngle(lat,lon,yaw);
	moveUntilClose(angle);
}

//________________Main________________
void setup() {
	Serial2.begin(9600); //for vision
	Serial1.begin(9600); //for GPS
	Serial3.begin(9600); //for compass
	askcubie("C", cubieconf);
	pinMode(LAMPADA,OUTPUT);
  calibration();
}

void loop() {
	while(1==1) {
		getGPS();
		float gpscurrent[2] = {gpsCurrentLat, gpsCurrentLon};
		if ((gpscurrent[0]-points[i] < gpserror)
			 && (gpscurrent[1]-points[i+3] < gpserror)) {
			//motion by vision
			char region = askcubie("Q",String(""));
			vismove(region);
			if (region == "S"){
				fire();
				i++;
				if (i>2) { goto FINISH; }
			}
		} else {
			//motion by gps
			gpsmove(gpscurrent[0],gpscurrent[1]);
		}
	}
	FINISH:
	unsigned long t = millis();
	String ts="";
	int i;
	for (i=1;i<=(7-length(t));i++) ts+="0"; //timer always 7 digits
	ts+=String(t);
	ts+=",.";
	askcubie("S", ts);
	while (1==1){delay(10000);}
}


