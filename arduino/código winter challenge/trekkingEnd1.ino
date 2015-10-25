//Código Trekking arduino Data : 23/01/2015

//Leitura do GPS e da IMU ,cálculo do ângulo funcionando.
//Comentado.
//Apenas para o primeiro ponto do campo.

// SEMEAR EESC USP 2015
#include <TinyGPS.h> //biblioteca necessária para leitura GPS
#include <Servo.h>

#define bROBOCORE true
#define DEBUG false

#define SPEED_KNOB 9
#define TURN_KNOB  10
#define ZERO 1500
#define STMIN 1000 //máximo valor para Sabertooth 2/12
#define STMAX 2000

#define FRENTE_VEL 0.65
#define DIF 0.2
#define BALIZA_VEL 0.6
#define BALIZA_DIF 0.2
#define BALIZA_TEMPO 800
#define BALIZA_TEMPO_CAM 500

#define LED_DEBUG 13
#define LAMPADA 12

#define Kmotor 0.01 //K = (2/180)= 0.005555
#define Kimotor 0.0005  

#define pi 3.14159265
#define limiarDist 5
#define angDireto 60
#define delayPerto 1000

//Não usados
//GPS
TinyGPS gps;
float lati=-23.646984 ;// localização inicial, aonde começa a prova. -22.006746	-47.898059 
float loni=-46.572814 ;
float angMeas,angApontado,angDif,angCerto,alpha,beta;
//float latp[3] = {-23.647010, -23.647228, -23.647027};
//float lonp[3] = {-46.572822, -46.572799, -46.572631};
//float latf=-23.647319,lonf=-46.572616; 
float latp[3] = {-23.647335, -23.647235, -23.647010};
float lonp[3] = {-46.572647, -46.5727818, -46.572639};
float latf = -23.647340,lonf=-46.572616;

int contadorPonto = 0, contadorAtolado = 0;
float distancia, distAntes;
float flatAntes=0,flonAntes=0;

float x1=37;// localização em metros do primeiro ponto em relação ao ponto inicial.
float y1=17;
float LatAnt;
float flat, flon; // localização atual do robô, lido do gps.

//IMU
float angRef =23; // angulo de ajuste da IMU, referência.
char yaw[15],pitchArray[15],dc[10],centrox[10];
float ang,pitch;

//Camera
int cx=-1,cy=-1,bCamPerto;
String rcv,test = "";

//Botao inicio
//Terminal C (via laranja) vai para pino 2
//Terminal NO (via verde) vai para GND
//Terminal NC (via laranaja/branca) vai para 5V
volatile int desligado = HIGH;

//Motores
Servo speedKnob, turnKnob;

int i=0;
char c;
float limiarAng;
int aux=0;
int auxtimer=0;
int aux2=0;
char direction ='z';
unsigned long tempo,tempoInicial;
int auxBaliza = 1;
float somaErro=0;

//Funções básicas de locomoção: 0<pot<1 e 0<dif<1
void Parar(void);
void Frente(float pot);
void Esquerda(float pot, float dif);
void Direita(float pot, float dif);
void trasEsquerda(float pot, float dif);
void trasDireita(float pot, float dif);
float direcaoPI(float, float, float);

unsigned long inicio;

// Parametros do filtro  cz
int cyA[5]={0,0,0,0,0};
int cyMax=0;
int cyMin=480;
//int cy;
int w;

//parametros para de tempo frenteperto
unsigned long tempoperto;
int auxtp=1;

void setup()
{
  Serial.begin(115200);// Serial comunicacao cubie
  Serial1.begin(9600);// Serial do GPS, Baud Rate 9600
  Serial2.begin(115200);// Serial para XBEE
  Serial3.begin(57600);// Serial IMU
  Serial.println("Trekking!!!");
  pinMode(LAMPADA,OUTPUT);
  pinMode(LED_DEBUG,OUTPUT);
  speedKnob.attach(SPEED_KNOB);
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
int contadorLuz = 0;

void loop()
{ 
 float latp[3] = {-23.647335, -23.647235, -23.647010};
 float lonp[3] = {-46.572647, -46.5727818, -46.572639};
 latf = latp[contadorLuz];
 lonf = lonp[contadorLuz];
  
  if(DEBUG)
  {
    lerGPS();
    impressao();
  }
  else
  {
    float gTurn;  
    // botao para parar programa
    while(desligado) Parar();
    //atoladoIMU();
    lerGPS ();
    Serial.println("leu gps de boa");
    lerCubie();
    Serial.println("leu cubie de boa");
    lerIMU ();
    Serial.println("leu imu de boa");
    orientacao ();
    Serial.println("se orientou de boa");
    impressao (); 
     
    //orientacao continua aqui
    if(distancia<limiarDist){
      Serial.println("camera");
      //chegarPlacaBranca();
      if(cy>170)
      {
        if(cx<(300)&& cx>0){
          fazBalizaAntiHor(BALIZA_VEL,BALIZA_DIF,BALIZA_TEMPO_CAM);
          Serial2.println("cam(ESQUERDA)");
        }
        if(cx>(340)){
          fazBalizaHor(BALIZA_VEL,BALIZA_DIF,BALIZA_TEMPO_CAM);
          Serial2.println("cam(DIRETA)");
        }
        if(cx<340 && cx>300){
          Serial.println("ta bem perto");
          auxBaliza = 1;
          Frente(0.65);
          delay(2000);
          Frente(-0.5);
          delay(100);
          Parar();
          PiscarLuz();
          Serial.println("chegou!!!!");
          contadorPonto=contadorPonto+1;
          //while(contadorPonto==3) Parar();
          latf=latp[contadorPonto];
          lonf =lonp[contadorPonto];
          Serial.println("saindo....");
      }
      }
      else{//placa branca longe ainda
        if(cx==-1){
          fazBalizaAntiHor(BALIZA_VEL,BALIZA_DIF,BALIZA_TEMPO_CAM);
        }
        if(cx<(250)&& cx>0){
          fazBalizaAntiHor(BALIZA_VEL,BALIZA_DIF,BALIZA_TEMPO_CAM);
          Serial2.println("cam(ESQUERDA)");
        }
        if(cx>(390)){
          fazBalizaHor(BALIZA_VEL,BALIZA_DIF,BALIZA_TEMPO_CAM);
          Serial2.println("cam(DIRETA)");
        }
        if(cx>(250) && cx<(390)){
          if(auxtp==1){
            tempoperto=millis();
            auxtp=0;}
            if(auxtp==0){
              Frente(0.8);
            if((tempoperto - millis()>800)){
              Parar();
            }
            if((tempoperto - millis()>1500)){
              auxtp=1;
            }
            }
            
          Serial2.println("cam(FRENTE)");
        }
      }
    }
    if(distancia > limiarDist)
    {
      if ((angDif > -angDireto)&&(angDif<angDireto))
      {
        Serial.println("GPS PI");
        gTurn = direcaoPI(Kmotor, Kimotor, angDif);
        imprimir(gTurn,2,"Prop gTurn: ",'\n');
        mov(FRENTE_VEL, gTurn);
      }
      else 
      { 
        Serial.println("GPS baliza");
        fazBalizaIMU(BALIZA_VEL,BALIZA_DIF,BALIZA_TEMPO);  
      } 
    }
  }//fecha if DEBUG
  Serial.print(contadorPonto);
 Serial.println("loop...");
}//fecha void loop


// será explicado melhor futuramente
float AchaAlpha (float x1,float y1,float lat0,float lon0,float lat1,float lon1)
{ 
  float dlat = lat1-lat0;
  float dlon = lon1-lon0;
  float alpha=0;
  alpha= atan2(dlat,dlon)-atan2(y1,x1);
  alpha=180*alpha/pi;
  alpha=to360(alpha);
  return alpha;
}

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

void chegarPlacaBranca()
{  
}

float direcaoPI(float Kp, float Ki, float erro)
{
  float auxK;
  auxBaliza=1;
  
    auxK = - (Kp * erro + Ki * somaErro);
  
  somaErro += erro;
  Serial.print("K*erro: ");
  Serial.println(auxK);
  if(auxK<-0.3)auxK = -0.3;
  if(auxK>0.3)auxK = 0.3;
  return auxK;
}

// Distância entre dois pontos .
float dist(float lat1, float lon1,float lat2,float lon2)
{
  float  R = 6371000; // km 
  float  lat1rad = lat1*pi/180;
  float  lat2rad = lat2*pi/180;
  float  lon1rad = lon1*pi/180;
  float  lon2rad = lon2*pi/180;
  float dlon= lon2rad - lon1rad;
  float dlat= lat2rad - lat1rad;
  float a=0,c=0;
  a= sin(dlat/2)*sin(dlat/2) + cos(lat1rad)*cos(lat2rad)*sin(dlon/2)*sin(dlon/2);
  c=2*atan2(sqrt(a),sqrt(1-a));
  float d = R*c;
  return d;
}
void fazBalizaHor(float pot, float dif, int tempoBaliza)
{
    if (auxBaliza==1){
      tempoInicial=millis();
      auxBaliza=0;
    }
    else auxBaliza=balizaHor(pot,dif,tempoBaliza); 
}
void fazBalizaAntiHor(float pot, float dif, int tempoBaliza)
{
    if (auxBaliza==1){
      tempoInicial=millis();
      auxBaliza=0;
      Frente(FRENTE_VEL);
      delay(200);
    }
    else auxBaliza=balizaAntiHor(pot,dif, tempoBaliza); 
}
void fazBalizaIMU(float pot, float dif, int tempoBaliza)
{
  if (angDif>0){        
    if (auxBaliza==1){
      tempoInicial=millis();
      auxBaliza=0;
      Frente(FRENTE_VEL);
      delay(200);
    }
    else auxBaliza=balizaAntiHor(pot,dif, tempoBaliza); 
  }
  else
  {
    if (auxBaliza==1){
    tempoInicial=millis();
    auxBaliza=0;
    }
    else auxBaliza=balizaHor(pot,dif, tempoBaliza);
  }  
}
void impressao ()
{
  imprimir(contadorPonto,0,"Ponto: ",' ');
  imprimir(distancia,2,"dist: ", ' ');
  imprimir(beta,2,"beta: ",' ');
  //imprimir(angCerto,2,"angCerto: ",' ');
  imprimir(angMeas,2,"angMeas: ",' ');
  //imprimir(angApontado,2,"angApontado: ",' ');
  imprimir(angDif,2,"Dif: ",' ');
  imprimir(flat,6,"Lat: ",' ');
  imprimir(flon,6,"Lon: ",' ');
  imprimir(cx,0,"cx: ",' ');
  imprimir(cy,0,"cy: ",'\n');
}
void imprimir(float variavel, int casas, String id, char complemento) 
{
  Serial.print(id);
  Serial.print(variavel,casas);
  Serial.print(complemento);
}
void interrupcaoBotao(){ desligado = !desligado;}

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

void lerGPS ()
{
  // ínicio leitura GPS
  Serial1.flush();
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 300;)
  {
    if (Serial1.available())
    {
      char c = Serial1.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    //Serial.println(flat);
  }

  gps.stats(&chars, &sentences, &failed);
  //fim leitura GPS  
  if(bROBOCORE)
  {
    if (flat< -23.647380 || flat > -23.646966){
      flat = flatAntes;
    }
    else{
      flatAntes=flat;
    }
    if (flon< -46.572853 || flon > -46.572563){
      flon =flonAntes;
    }
    else{
      flonAntes=flon;
    }
  }
}

void lerIMU ()
{
  //inicio leitura IMU
  Serial3.flush();// inicia a serial apenas quando for ler
  while(Serial3.available())
  {
    //Serial.println("while imu");
    c = Serial3.read(); // le "bit a bit" -0.38,:-33.56;-0.39,:-33.55;-0.38,:-33.54;-0.40,:
    if(aux==2){
      if(c!=','){
        pitchArray[i]= c; //string recebe c
        i+=1;
      } 
      if(c==','){
        pitchArray[i] = '\0';
        pitch = atof(pitchArray);
        i = 0;
        aux=0;
      }
    }
    if (aux==1){
      if(c!=';'){
        yaw[i]= c; //string recebe c
        i+=1;
      } 
      if(c==';'){
        yaw[i] = '\0';
        angMeas = atof(yaw);
        i = 0;
        aux=2;
      }
    }
    if (c==':'){
      aux=1; // espera aparecer ':' para o inicio da leitura.
    }
  }
  aux=0;
}


void orientacao ()
{
  distancia= dist(flat,flon,latf,lonf);  
  beta=AchaAlpha(10,0,flat,flon,latf,lonf);
  angCerto=to360(beta - alpha);
  angMeas=to360(angMeas);
  angApontado=to360(angRef-angMeas);// angulo apontado pelo robo
  angDif= to360(angCerto - angApontado); // diferença no plano xy global entre o ang do robo e o ang do proximo ponto.
  if(angDif > 180) angDif -= 360;
}
// transforma ângulos >360 ou <0 em ângulos entre 0 e 360 
float to360(float ang)
{
  float res=ang;
  if(ang < 0){
    res = 360+ang;
  }
  if(ang>360){
    res=ang-360;
  }
  return res;
}
int limitarTurn(int turn)
{
  if(turn > STMAX) return STMAX;
  if(turn < STMAX) return STMIN;
  return turn;
}
void mov(float fwd, float turn)// -1.0 < fwd < 1.0 e -1.0 < turn < 1.0
// turn > 0 => virar para esquerda
{
  speedKnob.writeMicroseconds(fwd*500+ZERO);
  turnKnob.writeMicroseconds(turn*500+ZERO); 
}
void Parar ()
{
  speedKnob.writeMicroseconds(ZERO);
  turnKnob.writeMicroseconds(ZERO);  
}
void  Frente (float pot)
{
  mov(pot,ZERO);
}
void Esquerda (float pot, float dif)
{
  mov(abs(pot),-abs(dif));
}
void Direita (float pot, float dif)
{
  mov(abs(pot),abs(dif));
}
void trasEsquerda (float pot, float dif)
{ 
  mov(-abs(pot),abs(dif));
}
void trasDireita (float pot, float dif)
{
  mov(-0.7,-0.2);
}

//BALIZAS
int balizaHor (float pot, float dif, int tempoBaliza){
  unsigned long tempo=millis();
  if (tempo-tempoInicial<=3*tempoBaliza){
    trasEsquerda (pot, dif);
    return 0;
  }
  if ((tempo-tempoInicial>3*tempoBaliza)&&(tempo-tempoInicial<=4*tempoBaliza)){
    Parar();
    return 0;
  }
  if ((tempo-tempoInicial>4*tempoBaliza)&&(tempo-tempoInicial<=6*tempoBaliza)){
    Direita(pot, dif);
    return 0;
  }
  if ((tempo-tempoInicial>6*tempoBaliza)&&(tempo-tempoInicial<=7*tempoBaliza)){
    Parar();
    return 0;
  }
  return 1;
}
int balizaAntiHor (float pot, float dif, int tempoBaliza){
  unsigned long tempo=millis();
  if (tempo-tempoInicial<=3*tempoBaliza){
    trasDireita (pot,dif);
    return 0;
  }
  if ((tempo-tempoInicial>3*tempoBaliza)&&(tempo-tempoInicial<=4*tempoBaliza)){
    Parar();
    return 0;
  }
  if ((tempo-tempoInicial>=4*tempoBaliza)&&(tempo-tempoInicial<=6*tempoBaliza)){
    Esquerda(pot,dif);
    return 0;
  }
  if ((tempo-tempoInicial>6*tempoBaliza)&&(tempo-tempoInicial<=7*tempoBaliza)){
    Parar();
    return 0;
  }
  return 1;
}

float angDifAntes;
void atoladoIMU()
{
  int intervalo = 5000;
  if((millis()-inicio)%intervalo>0.9*intervalo){
    if(abs(angDif-angDifAntes) < 3) contadorAtolado++;
    else contadorAtolado = 0;
    angDifAntes = angDif;
  }  
  if(contadorAtolado > 3){
    Serial.println("!!!!!!!!!ATOLADO!!!!!!!!!");
    contadorAtolado = 0;
    mov(-FRENTE_VEL,0);
    delay(2000);
  }  
  imprimir(contadorAtolado,0,"contadorAtolado: ",'\n');
}
void atoladoGPS()
{
  int intervalo = 5000;
  if((millis()-inicio)%intervalo>0.9*intervalo){
    if(abs(distancia-distAntes) < 0.5) contadorAtolado++;
    else contadorAtolado = 0;
    distAntes = distancia;
  }  
  if(contadorAtolado > 3){
    Serial.println("!!!!!!!!!ATOLADO!!!!!!!!!");
    contadorAtolado = 0;
    mov(-FRENTE_VEL,0);
    delay(2000);
  }  
  imprimir(contadorAtolado,0,"contadorAtolado: ",'\n');
}
