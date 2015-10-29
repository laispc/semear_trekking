#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(9600);
	// set the data rate for the SoftwareSerial port
	mySerial.begin(9600);
	mySerial.write("C");
	delay(1000);
	mySerial.write("320,240,000,240,100,200,050,020,070,080,080,0,.");
	delay(1000);
	Serial.write("Settings are sent to cubie");
}

void loop()
{

	mySerial.write("Q");
	
	while(1==1){
		if (mySerial.available()) {
			Serial.write(mySerial.read());
			break;
		}
	}
	
	Serial.write("\n");
	delay(1000);
}
