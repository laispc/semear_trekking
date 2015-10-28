#define MOTORRIGHT1 6
#define MOTORRIGHT2 5
#define MOTORLEFT1 4
#define MOTORLEFT2 3


//variáveis de controle e de motores
#define MAXIMUMMOTORPOWER 0  //potencia maxima dos motores (para uso da sabertooth, o sinal 0 corresponde à potencia máxima para frente, 255 para trás, e 128 parado)
#define DIFERENCEBETWEENMOTORS 64   //ddiferença máxima de potencia em sinal para locomoção
#define ANGLETHRESHOLD 30     //angulo de erro em que o robô começa a girar em torno do proprio eixo na locomoção
#define kp DIFERENCEBETWEENMOTORS/ANGLETHRESHOLD     //constante proporcional do controle PI
//#define ki=0,001;      //constante integrativa do controle PI
#define ANGLETHRESHOLD 30     //angulo de erro em que o robô começa a girar em torno do proprio eixo na locomoção
#define POWERFORWARD 50
//float sumError=0;    //auxiliar para calculo da integral do erro
float control=0;//saida do controle PI
int numberCubie=-1;


//variáveis gerais
#define LAMPADA 53
#define DEBUG 1   //0 para debugar ou 1 se não for debugar
int state=0;   //estado para o loop 
int lightCounter = 0;   //contador para o numero de vezes que a lampada ligou(numero de pontos em que se chegou)
int initialLoopTime, tempoFinal;   //temporizadores no código
float iniciallLoopTime;     //pega o tempo de inicio do loop fora a calibração
float currentTime;    //tempo atual para calculo da integral no controle PI


//variáveis GPS
TinyGPS gps;
float LATPOINT[3] = {-22.006420135, -22.006283787084158, -22.006701567303146};//coordenadas em latitude dos objetivos
float LONPOINT[3] = {-47.898288726, -47.898066490888596, -47.89872094988823};//coordenadas em longitude dos objetivos
float gpsCurrentLat, gpsCurrentLon;      //latitude e longitude atual
float lonCurrentTarget = LONPOINT[lightCounter];     //longitude do objetivo atual
float latCurrentTarget = LATPOINT[lightCounter];      //latitude do objetivo atual

//variáveis IMU

float yaw;
float yaw2;
char imuRead;
char yawArray[15];
float COMPASS_REFERENCE_ANGLE = 113;; 		// Compass reference angle.
float rotAngle = 0;
int i=0;					// Rotação necessária para alinhar robô com o objetivo

