#define MOTORRIGHT1 5
#define MOTORRIGHT2 6
#define MOTORLEFT1 5
#define MOTORLEFT2 6
//#define kp=0,1;
//#define ki=0,001;

#define ANGLETHRESHOLD 30
#define FIXMOTORRIGHT 0.95
#define FIXMOTORLEFT 1

#define LATPOINT[3] = {-23.647335, -23.647235, -23.647010};
#define LONPOINT[3] = {-46.572605, -46.572818, -46.572639};

#define DISTANCETRHESHOLD 3

#define DEBUG 1 //0 para debugar ou 1 se não for debugar

int state=0;
int lightCouter = 0;
int tempoInicial, tempoFinal;

float gpsCurrentLat, gpsCurrentLon;
latCurrentTarget = LATPOINT[lightcounter];
lonCurrentTarget = LONPOINT[lightcounter];

float distanceToTarget;

