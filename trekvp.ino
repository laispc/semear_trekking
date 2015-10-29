#include <TinyGPS.h>


//_____________Global_variables_____________
int i=0; //counter for check-points
float points[6] = {1,2,3,1,2,3}; //GPS data for check-points
String cubieconf = "320,240,000,240,100,200,050,020,070,080,080,0,.";
float gpsCurrentLat, gpsCurrentLon;
float gpserror = 0.00001;


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

void move(char r) {
	//move by vision
	
	
}

void amove(float a) {
	//move by gps
	
}

void fire() {
	//signal when on the point
	
	
}

//________________Main________________
void setup() {
	Serial2.begin(9600); //for vision
	Serial3.begin(9600); //for GPS
	Serial4.begin(9600); //for compass
	askcubie("C", cubieconf);
}

void loop() {
	while(1==1) {
		getGPS();
		float gpscurrent[2] = {gpsCurrentLat, gpsCurrentLon};
		if ((gpscurrent[0]-points[i] < gpserror)
			 && (gpscurrent[1]-points[i+3] < gpserror)) {
			//motion by vision
			char region = askcubie("Q",String(""));
			move(region);
			
			if (region == "S"){
				fire();
				i++;
				if (i>2) { goto FINISH; }
			}
		} else {
			//motion by gps
			
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
