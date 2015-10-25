// Parametros do filtro  cz
int cyA[5]={0,0,0,0,0};
int cyMax=0;
int cyMin=480;
//int cy;
int w;

int filtroCY(){  
  if(cy!=-1) {
    for (w=0;w<=4;w++){
      if(cyA[w]<cyMin) cyMin=cyA[w];
      if(cyA[w]>cyMax) cyMax=cyA[w];
      if(w!=0) cyA[w-1]=cyA[w];
      if(w==4) cyA[w]=cy;
    }
  }
  cy=(cyA[0]+cyA[1]+cyA[2]+cyA[3]+cyA[4]-cyMax-cyMin)/3;
  // restart values 
  cyMax=0;
  cyMin=480;
  return cy;
}

void lerCubie()
{
   Serial2.read();
   char val;        //received char from Cubie
   if (Serial2.available() > 0)
   {
     rcv = "";
     while (Serial2.available())
     {
       Serial.println("while da cubie"); 
       val = Serial2.read();
        if (val == '.')
        {
          //here we can work with received data
          int delim = rcv.indexOf(';');
          cx = rcv.substring(0, delim).toInt();
          cy = rcv.substring(delim+1).toInt();
          cy=filtroCY();  
        }
        rcv += val;
      }
   }
}
