#include <SoftwareSerial.h>
#include "./TinyGPS.h"             

TinyGPS gps;
SoftwareSerial nss(6, 255);            // GPS TXD to digital pin 6
float flat, flon;                      // vars for current GPS coordinates
float p1at = -22.010, p1on = -47.897;  // GPS of control points
float p2at = -22.010, p2on = -47.896; 
float p3at = -22.011, p3on = -47.896;

void setup() {
  Serial.begin(115200);
  nss.begin(9600);                     // Communicate at 9600 baud
}

void loop() {
  
  getgps();
  Serial.print(flat, 7); Serial.print(", "); Serial.println(flon, 7); //debug monitor



  delay(1000);
}

//=======================FUNCTIONS================================
void getgps(){
  bool newdata = false;
  unsigned long start = millis();
  while (millis() - start < 300) {    // msec to update gps data
    if (feedgps())  newdata = true;
  }
  if (newdata) {
    gpsdump(gps);
  }
}
// Get and process GPS data
void gpsdump(TinyGPS &gps) { 
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
}
// Feed data as it becomes available
bool feedgps() {
  while (nss.available()) {
    if (gps.encode(nss.read()))
      return true;
  }
  return false;
}

