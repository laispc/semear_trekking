void chegarPlacaBranca()
{  
  Serial.println("ta bem perto");
  auxBaliza = 1;
  Frente(0.45);
  delay(800);
  Frente(-0.2);
  delay(100);
  Parar();
  PiscarLuz();
  Serial.println("chegou!!!!");
  contadorPonto=contadorPonto+1;
  //while(contadorPonto==3) Parar();
  //latf=latp[contadorPonto];
  //lonf =lonp[contadorPonto];
  Serial.println("saindo....");
}

void interrupcaoBotao(){ desligado = !desligado;}

void PiscarLuz ()
{
  for(int lamps=0;lamps<=3;lamps++){
  digitalWrite (LAMPADA,HIGH);
  delay(500);
  digitalWrite (LAMPADA,LOW);
  delay(500);  
  }
  contadorLuz++;
}


