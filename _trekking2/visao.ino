
int readCubie()
{
   Serial2.read();
   char val;        //received char from Cubie
   if (Serial2.available() > 0)
   {
     int valueCubie=Serial2.read();
     return valueCubie;
   }
}
   
