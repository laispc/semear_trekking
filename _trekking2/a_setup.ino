
void setup()
{
  Serial.begin(115200);// Serial comunicacao com usuário
  Serial1.begin(9600);// Serial do GPS, Baud Rate 9600
  Serial2.begin(115200);// Serial para cubie
  Serial3.begin(57600);// Serial IMU
  Serial.println("Trekking!!!");
  pinMode(LAMPADA,OUTPUT);
 // pinMode(LED_DEBUG,OUTPUT);
  pinMode(MOTORRIGHT1, OUTPUT);
  pinMode(MOTORRIGHT2, OUTPUT);
  pinMode(MOTORLEFT1, OUTPUT);
  pinMode(MOTORLEFT2, OUTPUT);
}
  

