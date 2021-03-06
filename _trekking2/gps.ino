
//=======================FUNCTIONS================================
void getGPS(){
  bool newdata = false;
  unsigned long start = millis();
  while (millis() - start < 200) {    // msec to update gps data
    if (feedgps())  newdata = true;
  }
  if (newdata) {
    gpsdump(gps);
  }
  lonCurrentTarget = LONPOINT[lightCounter];     //longitude do objetivo atual
  latCurrentTarget = LATPOINT[lightCounter];      //latitude do objetivo atual
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

