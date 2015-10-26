//Código Trekking arduino Data : 23/01/2015

//Leitura do GPS e da IMU ,cálculo do ângulo funcionando.
//Comentado.
//Apenas para o primeiro ponto do campo.

// SEMEAR EESC USP 2015
#include "./TinyGPS.h"    

//#######################################################
//Para trabalhar com múltiplos arquivos, seguir algumas regras:
//
// 1. #include de biblioteca apenas no sketch principal (_trekking)
// 2. Sugestão de colocar os protótipos das funções no sketch 
//    principal (_trekking)
// 3. Adicionar novas abas usando New Tab na seta abaixo de Serial
//    Monitor ou Ctrl+Shift+N
// 4. As abas são concatenadas na hora de compilação. Portanto, um
//    código final comeca com o sketch principal (_trekking) e 
//    continua em ordem ASCII (A,B,...,Z,a,b...,z)
// 5. Sugestão de colocar os #define e variáveis globais em uma aba
//    Definitions (única em maiúscula)
// 6. Resto das abas em letra minúscula
// 7.`Também é possível colocar o void setup() e void loop() em abas
//    separadas. Como sugestão, nomeá-las como Z_loop e Z_setup.
//    Portanto, elas são as primeiras abas após Definições

      //controle dos motores para angulos pequenos
void turnCounterClocklwise();                         //controle dos motores para "angError" grandes (maiores que 45º)  ou quando está proximo do objetivo
void turnClockWise();                                 //controle dos motores para "angError" grandes (menores que -45º)     
void moveForward();
void stopMotors();

void calibration();
void motorsControl(float controlMotor);               //controle dos motores para "angError" pequenos
void moveUntilClose (float angleError);               //Função de locomoção para chegar até próximo da placa
void moveToTarget();
float readIMU();
void lightBlink ();
int readCubie();
void getGPS();

