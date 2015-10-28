#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  
}
int s=HIGH;
void loop() // run over and over
{

  mySerial.write("Q");
  while(1==1){
  if (mySerial.available()) {
    Serial.write(mySerial.read());
    break;
  }
  delay(100);
  }

  Serial.write("M\n");
  
  delay(1000);
}
