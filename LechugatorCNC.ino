#include <elapsedMillis.h>
#include "TeensyStep.h"
#include <Servo.h>

// #define LED_BUILTIN PA_5
HardwareSerial Serial1(PA_10, PA_9); // PA_10:RX - PA_9:TX

// PINES DE LOS MOTORES DEL EJE Y
constexpr int Mot_Pull_Y1 = PB_0, Mot_Dir_Y1 = PB_1, Mot_En_Y1 = PB_2;
constexpr int Mot_Pull_Y2 = PA_0, Mot_Dir_Y2 = PA_1, Mot_En_Y2 = PA_4;

// PINES DE LOS MOTORES DEL EJE Z
constexpr int Mot_Pull_Z1 = PB_3, Mot_Dir_Z1 = PB_4, Mot_En_Z1 = PB_5;
constexpr int Mot_Pull_Z2 = PA_5, Mot_Dir_Z2 = PA_6, Mot_En_Z2 = PA_7;

// PINES DE LOS MOTORES DEL EJE X
constexpr int Mot_Pull_X1 = PA_8, Mot_Dir_X1 = PA_13, Mot_En_X1 = PA_14;

// DECLARACION DE LOS MOTORES DEL EJE Y
Stepper MOTOR_Y1(Mot_Pull_Y1, Mot_Dir_Y1);
Stepper MOTOR_Y2(Mot_Pull_Y2, Mot_Dir_Y2);

// DECLARACION DE LOS MOTORES DEL EJE z
Stepper MOTOR_Z1(Mot_Pull_Z1, Mot_Dir_Z1);
Stepper MOTOR_Z2(Mot_Pull_Z2, Mot_Dir_Z2);

// DECLARACION DE LOS MOTORES DEL EJE X
Stepper MOTOR_X1(Mot_Pull_X1, Mot_Dir_X1);

// VELOCIDAD DEL PULSO
StepControl controller_1(10U);
StepControl controller_2(20U);

// VELOCIDAD DE MOVIMIENTO DEL EJE Y
int Vel_Y = 8900;
int Ace_Y = 100000;
int Pos_Y = 500; // (200 * 8)*35.3 DISTANCIA TOTAL

// VELOCIDAD DE MOVIMIENTO DEL EJE Z
int Vel_Z = 10000;
int Ace_Z = 100000;
int Pos_Z = 1000; // DISTANCIA TOTAL ((200*8)*154)
int lastPos_Z1 = 0;

// VELOCIDAD DE MOVIMIENTO DEL EJE X
int Vel_X = 10500;
int Ace_X = 100000;
int Pos_X = 4048; // DISTANCIA TOTAL (200*101)
int lastPos_X = 0;

// End Stop del Eje Y
int ENSTOP_Y1 = PB_6;
int ESTADO_ENDSTOP_Y1 = 0;
int ENSTOP_Y2 = PB_8;
int ESTADO_ENDSTOP_Y2 = 0;

// End Stop del Eje z
int ENSTOP_Z1 = PB_9;
int ESTADO_ENDSTOP_Z1 = 0;
int ENSTOP_Z2 = PB_10;
int ESTADO_ENDSTOP_Z2 = 0;

// End Stop del eje X
int ENSTOP_X = PC0;
int ESTADO_ENDSTOP_X = 0;

Servo Garra;
int Angulo1 = 130;
int Angulo2 = 105;

// Timer
elapsedMillis displayStopwatch = 0; // timing the display of the current position

int lastPos_1 = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    Serial.println("Simple Serial Stepper Example");
    Serial.println("(type h for help)");

    pinMode(Mot_En_Y1, OUTPUT);
    pinMode(Mot_En_Y2, OUTPUT);
    pinMode(Mot_En_Z1, OUTPUT);
    pinMode(Mot_En_Z2, OUTPUT);
    pinMode(Mot_En_X1, OUTPUT);

    pinMode(ENSTOP_Y1, INPUT);
    pinMode(ENSTOP_Y2, INPUT);
    pinMode(ENSTOP_Z1, INPUT);
    pinMode(ENSTOP_Z2, INPUT);
    pinMode(ENSTOP_X, INPUT);

    Garra.attach(PC6);

    // MOTOR DEL EJE Y
    MOTOR_Y1
        .setMaxSpeed(Vel_Y)
        .setAcceleration(Ace_Y);

    MOTOR_Y2
        .setMaxSpeed(Vel_Y)
        .setAcceleration(Ace_Y);

    // MOTOR DEL EJE Z
    MOTOR_Z1
        .setMaxSpeed(Vel_Z)
        .setAcceleration(Ace_Z);

    MOTOR_Z2
        .setMaxSpeed(Vel_Z)
        .setAcceleration(Ace_Z);

    MOTOR_X1
        .setMaxSpeed(Vel_X)
        .setAcceleration(Ace_X);

    // APAGAR MOTORES DEL EJE Y
    digitalWrite(Mot_En_Y1, HIGH);
    digitalWrite(Mot_En_Y2, HIGH);

    // APAGAR MOTORES DEL EJE Z
    digitalWrite(Mot_En_Z1, HIGH);
    digitalWrite(Mot_En_Z2, HIGH);

    // APAGAR MOTORES DEL EJE X
    digitalWrite(Mot_En_X1, HIGH);

    // Angulo inicial de la Garra
    Garra.write(Angulo1);
}

void loop()
{
    // handle input from pins ---------------------------------------------

    char Estado = Serial.read();

    if (displayStopwatch > 20)
    {
        displayStopwatch = 0;

        int currentPos = MOTOR_Z1.getPosition();
        if (currentPos != lastPos_Z1) // only display if it changed
        {
            lastPos_Z1 = currentPos;
            Serial.println(currentPos);
        }
    }

    // Lecturta del Final de carrera
    ESTADO_ENDSTOP_X = digitalRead(ENSTOP_X);
    ESTADO_ENDSTOP_Y1 = digitalRead(ENSTOP_Y1);
    ESTADO_ENDSTOP_Y2 = digitalRead(ENSTOP_Y2);
    ESTADO_ENDSTOP_Z1 = digitalRead(ENSTOP_Z1);
    ESTADO_ENDSTOP_Z2 = digitalRead(ENSTOP_Z2);
    // Serial.println(ESTADO_ENDSTOP_Z2);
    // Serial.println(ESTADO_ENDSTOP_Y2);

    Serial.print("Eje Y: ");
    Serial.println(MOTOR_Y1.getPosition());
    Serial.print("Eje X: ");
    Serial.println(MOTOR_X1.getPosition());
    Serial.print("Eje Z: ");
    Serial.println(MOTOR_Z1.getPosition());

    // Serial.println(Angulo1);

    // Posicion Maxima del eje x
    if (MOTOR_X1.getPosition() > 19500)
    {
        controller_1.emergencyStop();
        delay(10);
        MOTOR_X1.getPosition();
        digitalWrite(Mot_En_X1, HIGH);
    }

    // Posicion Mima del eje x
    if (MOTOR_X1.getPosition() < 0)
    {
        controller_1.emergencyStop();
        delay(10);
        MOTOR_X1.getPosition();
        digitalWrite(Mot_En_X1, HIGH);
    }

    // Posicion Maxima del eje Y
    if (MOTOR_Y1.getPosition() < -49000)
    {
        controller_1.emergencyStop();
        delay(10);
        MOTOR_Y1.getPosition();
        MOTOR_Y2.getPosition();
        digitalWrite(Mot_En_Y1, HIGH);
        digitalWrite(Mot_En_Y2, HIGH);
    }

    // Posicion Maxima del eje Y
    if (MOTOR_Y1.getPosition() > 0)
    {
        controller_1.emergencyStop();
        delay(10);
        MOTOR_Y1.getPosition();
        MOTOR_Y2.getPosition();
        digitalWrite(Mot_En_Y1, HIGH);
        digitalWrite(Mot_En_Y2, HIGH);
    }

    // Posicion Maxima del eje Z
    if (MOTOR_Z1.getPosition() > 246300)
    {
        controller_2.emergencyStop();
        delay(10);
        MOTOR_Z1.getPosition();
        MOTOR_Z2.getPosition();
        digitalWrite(Mot_En_Z1, HIGH);
        digitalWrite(Mot_En_Z2, HIGH);
    }

    // Posicion Minimo del eje Z
    if (MOTOR_Z1.getPosition() < 0)
    {
        controller_2.emergencyStop();
        delay(10);
        MOTOR_Z1.getPosition();
        MOTOR_Z2.getPosition();
        digitalWrite(Mot_En_Z1, HIGH);
        digitalWrite(Mot_En_Z2, HIGH);
    }

    // Apagar los Motores cuando no se esten moviendo del eje X y Y
    if (!controller_1.isRunning())
    {
        digitalWrite(Mot_En_X1, HIGH);
        digitalWrite(Mot_En_Y1, HIGH);
        digitalWrite(Mot_En_Y2, HIGH);
    }

    // Apagar los Motores cuando no se esten moviendo del eje Z
    if (!controller_2.isRunning())
    {
        digitalWrite(Mot_En_Z1, HIGH);
        digitalWrite(Mot_En_Z2, HIGH);
    }

    switch (Estado)
    {
    case 'W':
        Y_ADELANTE();
        Estado = 0;
        break;

    case 'S':
        Y_ATRAS();
        Estado = 0;
        break;

    case 'X':
        Y_Home();
        Estado = 0;
        break;

    case 'D':
        X_Derecha();
        Estado = 0;
        break;

    case 'A':
        X_Izquierda();
        Estado = 0;
        break;

    case 'Z':
        X_Home();
        Estado = 0;
        break;

    case 'R':
        Z_ARRIBA();
        Estado = 0;
        break;

    case 'F':
        Z_ABAJO();
        Estado = 0;
        break;

    case 'C':
        Z_Home();
        Estado = 0;
        break;

    case 'Q': // stop command
        STOP_MOTORES();
        Estado = 0;
        break;

    case 'V': // stop command
        Abrir_Gara();
        Estado = 0;
        break;

    case 'B': // stop command
        Cerar_Gara();
        Estado = 0;
        break;

    case 'G': // stop command
        Sembrar();
        Estado = 0;
        break;

    case 'H': // stop command
        Home();
        Estado = 0;
        break;

    case 'E': // emergency stop command
        controller_1.emergencyStop();
        controller_2.emergencyStop();
        digitalWrite(Mot_En_Y1, HIGH);
        digitalWrite(Mot_En_Y2, HIGH);
        Serial.println("Emergency Stop");
        Estado = 0;
        break;
    }
}

void Sembrar()
{
    Z_Home();
    delay(5);
    Y_Home();
    delay(5);
    X_Home();
    delay(5);
    // PRENDER MOTORES
    digitalWrite(Mot_En_X1, LOW);
    digitalWrite(Mot_En_Y1, LOW);
    digitalWrite(Mot_En_Y2, LOW);
    digitalWrite(Mot_En_Z1, LOW);
    digitalWrite(Mot_En_Z2, LOW);

    // Recorido
    Garra.write(Angulo2);
    delay(5);
    MOTOR_X1.setTargetRel(350);
    controller_1.move(MOTOR_X1);
    delay(5);
    MOTOR_Y1.setTargetRel(-15500);
    MOTOR_Y2.setTargetRel(15500);
    controller_1.move(MOTOR_Y1, MOTOR_Y2);
    delay(5);
    Sacar_Plantula();
    delay(5);
    Colocar_Plantual();
    
}

void Home()
{
    Z_Home();
    delay(5);
    Y_Home();
    delay(5);
    X_Home();
    delay(5);
    Garra.write(Angulo1);
    delay(5);
}

void Y_ADELANTE()
{
    digitalWrite(Mot_En_Y1, LOW);
    digitalWrite(Mot_En_Y2, LOW);
    MOTOR_Y1.setTargetRel(Pos_Y);
    MOTOR_Y2.setTargetRel(-Pos_Y);
    controller_1.moveAsync(MOTOR_Y1, MOTOR_Y2);
    MOTOR_Y1.getPosition();
    MOTOR_Y2.getPosition();
    Serial.println("MOTOR Y1 Y2 ADELANTE");
}

void Y_ATRAS()
{
    digitalWrite(Mot_En_Y1, LOW);
    digitalWrite(Mot_En_Y2, LOW);
    MOTOR_Y1.setTargetRel(-Pos_Y);
    MOTOR_Y2.setTargetRel(Pos_Y);
    controller_1.moveAsync(MOTOR_Y1, MOTOR_Y2);
    MOTOR_Y1.getPosition();
    MOTOR_Y2.getPosition();
    Serial.println("MOTOR Y1 Y2 ATRAS");
}

void Y_Home()
{

    digitalWrite(Mot_En_Y1, LOW);
    digitalWrite(Mot_En_Y2, LOW);
    MOTOR_Y1.setTargetRel(Pos_Y * 1000);
    MOTOR_Y2.setTargetRel(-Pos_Y * 1000);
    controller_1.moveAsync(MOTOR_Y1, MOTOR_Y2);

    while (!digitalRead(ENSTOP_Y2) && !digitalRead(ENSTOP_Y1))
        ;
    controller_1.emergencyStop();
    delay(10);
    digitalWrite(Mot_En_Y1, HIGH);
    digitalWrite(Mot_En_Y2, HIGH);
    MOTOR_Y1.setPosition(0);
    MOTOR_Y2.setPosition(0);
    Serial.println("Parar Y2");
}

void Z_ARRIBA()
{
    digitalWrite(Mot_En_Z1, LOW);
    digitalWrite(Mot_En_Z2, LOW);
    MOTOR_Z1.setTargetRel(-Pos_Z);
    MOTOR_Z2.setTargetRel(-Pos_Z);
    controller_2.moveAsync(MOTOR_Z1, MOTOR_Z2);
    MOTOR_Z1.getPosition();
    MOTOR_Z2.getPosition();
    Serial.println("MOTOR Z1 Z2 ATRAS");
}

void Z_ABAJO()
{

    digitalWrite(Mot_En_Z1, LOW);
    digitalWrite(Mot_En_Z2, LOW);
    MOTOR_Z1.setTargetRel(Pos_Z);
    MOTOR_Z2.setTargetRel(Pos_Z);
    controller_2.moveAsync(MOTOR_Z1, MOTOR_Z2);
    MOTOR_Z1.getPosition();
    MOTOR_Z2.getPosition();
    Serial.println("MOTOR Z1 Z2 ATRAS");
}

void Z_Home()
{

    digitalWrite(Mot_En_Z1, LOW);
    digitalWrite(Mot_En_Z2, LOW);
    MOTOR_Z1.setTargetRel(-Pos_Z * 1000);
    MOTOR_Z2.setTargetRel(-Pos_Z * 1000);
    controller_2.moveAsync(MOTOR_Z1, MOTOR_Z2);

    while (digitalRead(ENSTOP_Z2) && !digitalRead(ENSTOP_Z1))
        ;

    controller_2.emergencyStop();
    delay(10);
    digitalWrite(Mot_En_Z1, HIGH);
    digitalWrite(Mot_En_Z2, HIGH);
    MOTOR_Z1.setPosition(0);
    MOTOR_Z2.setPosition(0);
    Serial.println("Parar Z2 Y Z1");
}

void X_Derecha()
{

    digitalWrite(Mot_En_X1, LOW);
    MOTOR_X1.setTargetRel(Pos_X);
    controller_1.moveAsync(MOTOR_X1);
    MOTOR_X1.getPosition();
    Serial.println(MOTOR_X1.getPosition());
    Serial.println("MOTOR X IZ");
}

void X_Izquierda()
{

    digitalWrite(Mot_En_X1, LOW);
    MOTOR_X1.setTargetRel(-Pos_X);
    controller_1.moveAsync(MOTOR_X1);
    MOTOR_X1.getPosition();
    Serial.println(MOTOR_X1.getPosition());
    Serial.println("MOTOR X DE");
}

void X_Home()
{

    // Serial.println(ESTADO_ENDSTOP_X);
    digitalWrite(Mot_En_X1, LOW);
    MOTOR_X1.setTargetRel(-Pos_X * 120);
    controller_1.moveAsync(MOTOR_X1);

    while (!digitalRead(ENSTOP_X))
        ;

    controller_1.emergencyStop();
    delay(10);
    digitalWrite(Mot_En_X1, HIGH);
    MOTOR_X1.setPosition(0);
    Serial.println("Parar X");

    // Serial.println(ESTADO_ENDSTOP_X);

    // while (ESTADO_ENDSTOP_X == 0)
    // {
    //     digitalWrite(Mot_En_X1, LOW);
    //     MOTOR_X1.setTargetRel(-Pos_X);
    //     controller_1.move(MOTOR_X1);
    //     if (digitalRead(ENSTOP_X))
    //     {
    //         break;
    //     }
    // }
    // digitalWrite(Mot_En_X1, HIGH);
    // controller_1.stop();
    // Serial.println("parar");
}

void STOP_MOTORES()
{

    controller_1.stop();
    controller_2.stop();

    digitalWrite(Mot_En_Y1, HIGH);
    digitalWrite(Mot_En_Y2, HIGH);
    digitalWrite(Mot_En_Z1, HIGH);
    digitalWrite(Mot_En_Z2, HIGH);
    digitalWrite(Mot_En_X1, HIGH);

    Serial.println("Detener motores");
}

void Abrir_Gara()
{
    //Angulo1 = Angulo1 - 10;
    Garra.write(Angulo2);
}

void Cerar_Gara()
{
    //Angulo1 = Angulo1 + 10;
    Garra.write(Angulo1);
}

void Sacar_Plantula(){

    MOTOR_Z1.setTargetRel(25000);
    MOTOR_Z2.setTargetRel(25000);
    controller_2.move(MOTOR_Z1, MOTOR_Z2);
    delay(5);
    Garra.write(113);
    delay(5);
    MOTOR_Z1.setTargetRel(9000);
    MOTOR_Z2.setTargetRel(9000);
    controller_2.move(MOTOR_Z1, MOTOR_Z2);
    delay(5);
    Garra.write(117);
    delay(5);
    MOTOR_Z1.setTargetRel(9000);
    MOTOR_Z2.setTargetRel(9000);
    controller_2.move(MOTOR_Z1, MOTOR_Z2);
    delay(5);
    Garra.write(123);
    delay(5);
    MOTOR_Z1.setTargetRel(9000);
    MOTOR_Z2.setTargetRel(9000);
    controller_2.move(MOTOR_Z1, MOTOR_Z2);
    delay(5);
    Z_Home();
    delay(5);
    Garra.write(Angulo1);
    delay(5);
    Y_Home();
    delay(5);
    

}

void Colocar_Plantual(){

    // PRENDER MOTORES
    digitalWrite(Mot_En_X1, LOW);
    digitalWrite(Mot_En_Z1, LOW);
    digitalWrite(Mot_En_Z2, LOW);
    delay(5);
    X_Home();
    delay(5);
    MOTOR_X1.setTargetRel(4048);
    controller_1.move(MOTOR_X1);
    delay(5);
    MOTOR_Z1.setTargetRel(24500);
    MOTOR_Z2.setTargetRel(24500);
    controller_2.move(MOTOR_Z1, MOTOR_Z2);
    delay(5);
    Garra.write(90);
    delay(5);
    Z_Home();
    delay(5);
    Garra.write(Angulo1);
    X_Home();
    delay(5);
}
// void Mover_Motores_Tiempo(){
// // MOVER MOTOR Y1 PARA DELANTE
//     if (!controller_1.isRunning())
//     {
//         digitalWrite(Mot_En_Y1, LOW);
//         MOTOR_Y1.setTargetRel(Pos_Y);
//         controller_1.moveAsync(MOTOR_Y1);
//         MOTOR_Y1.getPosition();
//         Serial.println("MOTOR Y1 ADELANTE");
//     }
//     // MOVER MOTOR Y2 para delante
//     if (!controller_1.isRunning())
//     {
//         digitalWrite(Mot_En_Y2, LOW);
//         MOTOR_Y2.setTargetRel(-Pos_Y);
//         controller_1.moveAsync(MOTOR_Y2);
//         MOTOR_Y2.getPosition();
//         Serial.println("MOTOR Y2 ADELANTE");
//     }
// }