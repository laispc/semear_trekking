
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
  gps.f_get_position(&gpsCurrentLat, &gpsCurrentLon, &age);
}
// Feed data as it becomes available
bool feedgps() {
  while (Serial1.available()) {
    if (gps.encode(Serial1.read()))
      return true;
  }
  return false;
}

