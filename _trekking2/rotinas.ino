void lightBlink ()
{
  digitalWrite (LAMPADA,HIGH);
  delay(500);
  digitalWrite (LAMPADA,LOW);
  delay(500);  
  lightCounter++;
}


