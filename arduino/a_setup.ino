
void setup()
{
  Serial.begin(115200);// Serial comunicacao cubie
  Serial1.begin(9600);// Serial do GPS, Baud Rate 9600
  Serial2.begin(115200);// Serial para XBEE
  Serial3.begin(57600);// Serial IMU
  Serial.println("Trekking!!!");
  pinMode(LAMPADA,OUTPUT);
  pinMode(LED_DEBUG,OUTPUT);
  pinMode(motorDireito1, OUTPUT);
  pinMode(motorDireito2, OUTPUT);
  pinMode(motorEsquerdo1, OUTPUT);
  pinMode(motorEsquerdo2, OUTPUT);
    
  turnKnob.attach(TURN_KNOB);
  Parar();
  alpha=AchaAlpha(x1,y1,lati,loni,latf,lonf);
  angRef=to360(angRef); // angulo de referencia
  attachInterrupt(0, interrupcaoBotao, FALLING);
  interrupts();
  rcv.reserve(200);
  
  while(desligado){
    lerGPS();
    if(flat==0 || flon==0)digitalWrite(LED_DEBUG,LOW);
    else digitalWrite(LED_DEBUG,HIGH);
  }
  inicio = millis();
}
