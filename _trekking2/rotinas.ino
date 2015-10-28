void lightBlink ()
{
  digitalWrite (LAMPADA,HIGH);
  delay(2000);
  digitalWrite (LAMPADA,LOW);
  delay(500);  
  lightCounter++;
  lonCurrentTarget = LONPOINT[lightCounter];     //longitude do objetivo atual
  latCurrentTarget = LATPOINT[lightCounter];      //latitude do objetivo atual

}
