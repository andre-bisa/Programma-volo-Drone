#include <Servo.h> 
#include <Wire.h>

#define MOTORI 1
#define DEBUG 0
#define DEBUG_DELAY_TIME 500

#define VALORE_MINIMO 58    // MIN 58
#define VALORE_MASSIMO 160  // MAX 160!!


// PIN 4 auto-configurazione
#define COEFF_PIN4 0.95      // coefficiente di "indebolimento" motori
int iniz_PIN4 = 0;           // valore della prima lettura
bool letto_PIN4 = 0;         // indica se ho letto o meno il valore la prima volta

// PIN 5 auto-configurazione
#define COEFF_PIN5 0.95      // coefficiente di "indebolimento" motori
int iniz_PIN5 = 0;           // valore della prima lettura
bool letto_PIN5 = 0;         // indica se ho letto o meno il valore la prima volta

// PIN 6 auto-configurazione
#define COEFF_PIN6 0.98      // coefficiente di "indebolimento" motori
int iniz_PIN6 = 0;           // valore della prima lettura
bool letto_PIN6 = 0;         // indica se ho letto o meno il valore la prima volta

// Giroscopio X auto-configurazione
#define COEFF_GIROX 0.98      // coefficiente di "indebolimento" motori
int iniz_GiroX = 0;           // valore della prima lettura
bool letto_GiroX = 0;         // indica se ho letto o meno il valore la prima volta

// Giroscopio Y auto-configurazione
#define COEFF_GIROY 0.98      // coefficiente di "indebolimento" motori
int iniz_GiroY = 0;           // valore della prima lettura
bool letto_GiroY = 0;         // indica se ho letto o meno il valore la prima volta

int PosServo_X;               // usate per convertire il valore letto in un valore da 0 a 179
int PosServo_Y;               // usate per convertire il valore letto in un valore da 0 a 179


const int MPU=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

Servo motore1; // PIN 3
Servo motore2; // PIN 9
Servo motore3; // PIN 10
Servo motore4; // PIN 11

void attacca();                      // AZIONO I MOTORI
void stacca();                       // STACCO I MOTORI
void attivaMotori();                 // ACCENDO I MOTORI, SOLO IN SETUP!!
void calcolaPIN4();                  // DA VALORI AI MOTORI PER SINISTRA/DESTRA
void calcolaPIN5();                  // DA VALORI AI MOTORI PER AVANTI/INDIETRO
void calcolaPIN6();                  // DA VALORI AI MOTORI PER ROTAZIONE SU SE STESSO
void aggiustaX();                    // CON IL GIROSCOPIO AGGIUSTO IL VOLO
void aggiustaY();                    // CON IL GIROSCOPIO AGGIUSTO IL VOLO

bool attaccato = 0;                 // 1= MOTORI ACCESI
bool manovraInCorso = 0;            // SE FACCIO LA MANOVRA NON DEVO AGGIUSTARE LA POSIZIONE

void autoAtterraggio(); // FUNZIONE PER FARLO ATTERRARE DA SOLO
void leggiGiroscopio(); // LEGGE I DATI DAL GIROSCOPIO

int gas = 0; // VARIABILE GLOBALE

// VALORI DA PASSARE AI MOTORI
volatile int valore1;
volatile int valore2;
volatile int valore3;
volatile int valore4;

// VARIABILI PER DETERMINARE VALORI
volatile double temp1;
volatile double temp2;
volatile double temp3;
volatile double temp4;

const int pin2 = 2;     // LEVETTA SX SU/GIU      (GAS)
const int pin4 = 4;     // LEVETTA DX SX/DX
const int pin5 = 5;     // LEVETTA DX SU/GIU
const int pin6 = 6;     // LEVETTA SX SX/DX
const int modalita = 7; // MODALITA (0 / 1 / 2) (PIN)

/********** MODALITA **********
 * numero | valore | descrizione
 *   0       1880~   Auto atterraggio
 *   1       1585~   Motori accesi
 *   2       1090~   Motori spenti
 ********** MODALITA **********/

// LETTURA RICEVENTE
// VEDI  pin2, pin4, pin5, pin6, modalita
volatile unsigned long results_2;
volatile unsigned long results_4;
volatile unsigned long results_5;
volatile unsigned long results_6;
volatile unsigned long results_7;



void setup() {
  pinMode(pin2, INPUT);
  pinMode(pin4, INPUT);
  pinMode(pin5, INPUT);
  pinMode(pin6, INPUT);
  pinMode(modalita, INPUT);

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  Serial.begin(9600);

  #if MOTORI==1
    attivaMotori();
  #endif;
    
}

void stacca()
{
  attaccato = 0;
  #if MOTORI==1
    motore1.detach();
    motore2.detach();
    motore3.detach();
    motore4.detach();
  #endif;
}

void attacca()
{
  attaccato = 1;
  #if MOTORI==1
    motore1.attach(3);
    motore2.attach(9);
    motore3.attach(10);
    motore4.attach(11);
  #endif;
}

void attivaMotori()
{
  int pos;
  // MOTORE 1
  motore1.attach(3);
  for(pos = 0; pos < 60; pos += 1)
  {
    motore1.write(pos);
    delay(15);
  }
  for(pos = 60; pos>=1; pos-=1)
  {
    motore1.write(pos);
    delay(15);
  }
  motore1.detach();
  // MOTORE 2
  motore2.attach(9);
  for(pos = 0; pos < 60; pos += 1)
  {
    motore2.write(pos);
    delay(15);
  }
  for(pos = 60; pos>=1; pos-=1)
  {
    motore2.write(pos);
    delay(15);
  }
  motore2.detach();
  // MOTORE 3
  motore3.attach(10);
  for(pos = 0; pos < 60; pos += 1)
  {
    motore3.write(pos);
    delay(15);
  }
  for(pos = 60; pos>=1; pos-=1)
  {
    motore3.write(pos);
    delay(15);
  }
  motore3.detach();
  // MOTORE 4
  motore4.attach(11);
  for(pos = 0; pos < 60; pos += 1)
  {
    motore4.write(pos);
    delay(15);
  }
  for(pos = 60; pos>=1; pos-=1)
  {
    motore4.write(pos);
    delay(15);
  }
  motore4.detach();
}

void autoAtterraggio()
{
  int i = gas;
  if (gas > 300)
  {
      for (i = gas; i >= 200; i = i-5)
      {
        leggiGiroscopio();
        temp1 = i;
        temp2 = i;
        temp3 = i;
        temp4 = i;
        aggiustaX();
        aggiustaY();
        valore1 = map(temp1, 0, 1000, 50, 100);
        valore2 = map(temp2, 0, 1000, 50, 100);
        valore3 = map(temp3, 0, 1000, 50, 100);
        valore4 = map(temp4, 0, 1000, 50, 100);
        motore1.write(valore1);
        motore2.write(valore2);
        motore3.write(valore3);
        motore4.write(valore4);
        Serial.println(i);
        delay(150);
      }
  }
  for (i = i; i >= VALORE_MINIMO; i = i-2)
  {
    leggiGiroscopio();
    temp1 = i;
    temp2 = i;
    temp3 = i;
    temp4 = i;
    aggiustaX();
    aggiustaY();
    valore1 = map(temp1, 0, 1000, 50, 100);
    valore2 = map(temp2, 0, 1000, 50, 100);
    valore3 = map(temp3, 0, 1000, 50, 100);
    valore4 = map(temp4, 0, 1000, 50, 100);
    motore1.write(valore1);
    motore2.write(valore2);
    motore3.write(valore3);
    motore4.write(valore4);
    Serial.println(i);
    delay(100);
  }
  // stacco perchè sono atterrato
  stacca();
}


void calcolaPIN4()
{
  // Differenza min-max: 760
  // Differenza min-centrale: 380
  // Differenza centrale-max: 380
  if (results_4 < (iniz_PIN4*0.99))
  {
    temp1 = temp1 * (((COEFF_PIN4 - 1)*results_4 + (-380 + iniz_PIN4 * (1 - COEFF_PIN4)))/(-380));
    temp2 = temp2;
    temp3 = temp3;
    temp4 = temp4 * (((COEFF_PIN4 - 1)*results_4 + (-380 + iniz_PIN4 * (1 - COEFF_PIN4)))/(-380));
    manovraInCorso = 1;
  } else if (results_4 > (iniz_PIN4*1.01))
  {
    temp1 = temp1;
    temp2 = temp2 * (((COEFF_PIN4 - 1) * results_4 + (380 + iniz_PIN4 * (1 - COEFF_PIN4)))/(380));
    temp3 = temp3 * (((COEFF_PIN4 - 1) * results_4 + (380 + iniz_PIN4 * (1 - COEFF_PIN4)))/(380));
    temp4 = temp4;
    manovraInCorso = 1;
  }
}

void calcolaPIN5()
{
  // Differenza min-max: 795
  // Differenza min-centrale: 391
  // Differenza centrale-max: 404
  if (results_5 < (iniz_PIN5*0.99))
  {
    temp1 = temp1;
    temp2 = temp2 * (((COEFF_PIN5 - 1) * results_5 + (-391 + iniz_PIN5 * (1 - COEFF_PIN5))) / (-391));
    temp3 = temp3;
    temp4 = temp4 * (((COEFF_PIN5 - 1) * results_5 + (-391 + iniz_PIN5 * (1 - COEFF_PIN5))) / (-391));
    manovraInCorso = 1;
  } else if (results_5 > (iniz_PIN5*1.01))
  {
    temp1 = temp1 * (((COEFF_PIN5 - 1) * results_5 + (404 + iniz_PIN5 * (1 - COEFF_PIN5))) / (404));
    temp2 = temp2;
    temp3 = temp3 * (((COEFF_PIN5 - 1) * results_5 + (404 + iniz_PIN5 * (1 - COEFF_PIN5))) / (404));
    temp4 = temp4;
    manovraInCorso = 1;
  }
}

void calcolaPIN6()
{
  // Differenza min-max: 765
  // Differenza min-centrale: 396
  // Differenza centrale-max: 369
  if (results_6 <= 1490)
  {
    temp1 = temp1 * (((COEFF_PIN6 - 1) * results_6 + (-396 + iniz_PIN6 * (1 - COEFF_PIN6))) / (-396));
    temp2 = temp2 * (((COEFF_PIN6 - 1) * results_6 + (-396 + iniz_PIN6 * (1 - COEFF_PIN6))) / (-396));
    temp3 = temp3;
    temp4 = temp4;
  } else if (results_6 >= 1520)
  {
    temp1 = temp1;
    temp2 = temp2;
    temp3 = temp3 * (((COEFF_PIN6 - 1) * results_6 + (369 + iniz_PIN6 * (1 - COEFF_PIN6))) / (369));
    temp4 = temp4 * (((COEFF_PIN6 - 1) * results_6 + (369 + iniz_PIN6 * (1 - COEFF_PIN6))) / (369));
  }
}

/*void aggiustaX()
{
  if (manovraInCorso) return;
  // Differenza min-max: 8000
  // Differenza min-centrale: 4000
  // Differenza centrale-max: 4000
  if (AcX <= (iniz_GiroX * 0.95))
  {
    temp1 = temp1 * (((COEFF_GIROX - 1) * AcX + (-4000 + iniz_GiroX * (1 - COEFF_GIROX))) / (-4000));
    temp2 = temp2;
    temp3 = temp3;
    temp4 = temp4 * (((COEFF_GIROX - 1) * AcX + (-4000 + iniz_GiroX * (1 - COEFF_GIROX))) / (-4000));
  } else if (AcX >= (iniz_GiroX * 1.05))
  {
    temp1 = temp1;
    temp2 = temp2 * (((COEFF_GIROX - 1) * AcX + (4000 + iniz_GiroX * (1 - COEFF_GIROX))) / (4000));
    temp3 = temp3 * (((COEFF_GIROX - 1) * AcX + (4000 + iniz_GiroX * (1 - COEFF_GIROX))) / (4000));
    temp4 = temp4;
  }
}*/

void aggiustaX()
{
  if (manovraInCorso) return;
  if (PosServo_X  < (iniz_GiroX * 0.98))
  {
    temp1 = temp1 * (((COEFF_GIROX - 1) * PosServo_X + (-30 + iniz_GiroX * (1 - COEFF_GIROX))) / (-30));
    temp2 = temp2;
    temp3 = temp3;
    temp4 = temp4 * (((COEFF_GIROX - 1) * PosServo_X + (-30 + iniz_GiroX * (1 - COEFF_GIROX))) / (-30));
  } else if (PosServo_X  >= (iniz_GiroX * 1.0))
  {
    temp1 = temp1;
    temp2 = temp2 * (((COEFF_GIROX - 1) * PosServo_X + (30 + iniz_GiroX * (1 - COEFF_GIROX))) / (30));
    temp3 = temp3 * (((COEFF_GIROX - 1) * PosServo_X + (30 + iniz_GiroX * (1 - COEFF_GIROX))) / (30));
    temp4 = temp4;
  }
}

void aggiustaY()
{
  if (manovraInCorso) return;
    if (PosServo_Y <= (iniz_GiroY * 0.98))
  {
    temp1 = temp1;
    temp2 = temp2 * (((COEFF_GIROY - 1) * PosServo_Y + (-30 + iniz_GiroY * (1 - COEFF_GIROY))) / (-30));
    temp3 = temp3;
    temp4 = temp4 * (((COEFF_GIROY - 1) * PosServo_Y + (-30 + iniz_GiroY * (1 - COEFF_GIROY))) / (-30));
  } else if (PosServo_Y >= (iniz_GiroY * 1.02))
  {
    temp1 = temp1 * (((COEFF_GIROY - 1) * PosServo_Y + (30 + iniz_GiroY * (1 - COEFF_GIROY))) / (30));
    temp2 = temp2;
    temp3 = temp3 * (((COEFF_GIROY - 1) * PosServo_Y + (30 + iniz_GiroY * (1 - COEFF_GIROY))) / (30));
    temp4 = temp4;
  }
}

/*void aggiustaY()
{
  if (manovraInCorso) return;
  // Differenza min-max: 3400
  // Differenza min-centrale: 1700
  // Differenza centrale-max: 1700
    if (AcY <= (iniz_GiroY * 0.95))
  {
    temp1 = temp1;
    temp2 = temp2 * (((COEFF_GIROY - 1) * AcY + (-1700 + iniz_GiroY * (1 - COEFF_GIROY))) / (-1700));
    temp3 = temp3;
    temp4 = temp4 * (((COEFF_GIROY - 1) * AcY + (-1700 + iniz_GiroY * (1 - COEFF_GIROY))) / (-1700));
  } else if (AcY >= (iniz_GiroY * 1.05))
  {
    temp1 = temp1 * (((COEFF_GIROY - 1) * AcY + (1700 + iniz_GiroY * (1 - COEFF_GIROY))) / (1700));
    temp2 = temp2;
    temp3 = temp3 * (((COEFF_GIROY - 1) * AcY + (1700 + iniz_GiroY * (1 - COEFF_GIROY))) / (1700));
    temp4 = temp4;
  }
}*/

void leggiGiroscopio()
{
  // CALCOLI PER GIROSCOPIO
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

void loop() {
  // LEGGO GIROSCOPIO
  leggiGiroscopio();

  PosServo_X = map(AcX, -16000, 16000, 0, 179);
  PosServo_Y = map(AcY, -16000, 16000, 0, 179);
  
  if (!letto_GiroX) {
    iniz_GiroX = PosServo_X;
    letto_GiroX = 1;
  }

  if (!letto_GiroY) {
    iniz_GiroY = PosServo_Y;
    letto_GiroY = 1;
  }
  
  
  // LEGGO DALLA RICEVENTE
  results_2 = pulseIn(pin2,HIGH);
  results_4 = pulseIn(pin4,HIGH);
  results_5 = pulseIn(pin5,HIGH);
  results_6 = pulseIn(pin6,HIGH);
  results_7 = pulseIn(modalita,HIGH);
  //results_2 = pulseIn(pin2,HIGH,25000);

  if ((results_4 > 500)&&(!letto_PIN4)) {
    iniz_PIN4 = results_4;
    letto_PIN4 = 1;
  }
  if ((results_5 > 500)&&(!letto_PIN5)) {
    iniz_PIN5 = results_5;
    letto_PIN5 = 1;
  }
  if ((results_6 > 500)&&(!letto_PIN6)) {
    iniz_PIN6 = results_6;
    letto_PIN6 = 1;
  }
  
  // fine aggiunta in corso

  // variabili da resettare
  gas = 0;
  manovraInCorso = 0;
  
  // converto e ottengo il valore del "gas"
  gas = map(results_2, 1070, 1900, 0, 1000);

  if ((attaccato) && (results_4 < 5000) && (results_5 < 500) && (results_6 < 500) && (results_7 < 500))
  {
    autoAtterraggio();
    goto fine;
  }

  // DO "GAS" AI MOTORI
  temp1 = gas;
  temp2 = gas;
  temp3 = gas;
  temp4 = gas;

  //calcolo valori singoli motori per potermi spostare con l'analogico di destra
  calcolaPIN4(); // DX/SX
  calcolaPIN5(); // AVANTI/INDIETRO
  calcolaPIN6(); // ROTAZIONE SU ME STESSO

  if (!manovraInCorso) // AGGIUSTO SOLO SE NON MI STO MUOVENDO (cosi da non ostacolare i movimenti)
  {
    aggiustaX();
    aggiustaY();
  }
  
  //double temp1 = gas * ((-0.05 * results_5 + 481)/405) * ((-0.05*results_4-325.9)/(-402)) ;
  //valore1 = map(temp1, 0, 1000, 50, 100);

  
  // ARMO / DISARMO
  /*if (results_6 > 1800) 
  {
    attacca(); // ATTIVO I MOTORI
  } else if (results_6 < 1400)
  {
    stacca(); // DISATTIVO I MOTORI
  }*/

  // CONFIGURAZIONE DELLE MODALITA
  if ((results_7 >= 1700) && (attaccato)) // AUTOATTERRAGGIO
  {
    autoAtterraggio();
    goto fine;
  } else if (results_7 <= 1200) // MOTORI SPENTI (DISARMO)
  {
    stacca();
  } else if ((results_7 >= 1500) && (results_7 <=1600)) // ACCENDO I MOTORI (ARMO)
  {
    attacca(); // ATTIVO I MOTORI
  }

  if ((temp1/gas > 0.99) && (temp1/gas < 1))
    temp1 = gas;
  if ((temp2/gas > 0.99) && (temp2/gas < 1))
    temp2 = gas;
  if ((temp3/gas > 0.99) && (temp3/gas < 1))
    temp3 = gas;
  if ((temp4/gas > 0.99) && (temp4/gas < 1))
    temp4 = gas;

  valore1 = map(temp1, 0, 1000, VALORE_MINIMO, VALORE_MASSIMO);
  valore2 = map(temp2, 0, 1000, VALORE_MINIMO, VALORE_MASSIMO);
  valore3 = map(temp3, 0, 1000, VALORE_MINIMO, VALORE_MASSIMO);
  valore4 = map(temp4, 0, 1000, VALORE_MINIMO, VALORE_MASSIMO);

  #if MOTORI==1
  // se sono armato aziono i motori
  if (attaccato)
  {
    motore1.write(valore1);
    motore2.write(valore2);
    motore3.write(valore3);
    motore4.write(valore4);
  }
  #endif;

  
fine:
  #if DEBUG==1
    //Serial.println(valore1);
    //Serial.println(attaccato);
    
    Serial.println("------------------------");
    Serial.print("Motore 1: ");
    Serial.print(valore1, DEC);
    Serial.print("    ");
    Serial.println((temp1/gas), DEC);
  
    Serial.print("Motore 2: ");
    Serial.print(valore2, DEC);
    Serial.print("    ");
    Serial.println((temp2/gas), DEC);
  
    Serial.print("Motore 3: ");
    Serial.print(valore3, DEC);
    Serial.print("    ");
    Serial.println((temp3/gas), DEC);
  
    Serial.print("Motore 4: ");
    Serial.print(valore4, DEC);
    Serial.print("    ");
    Serial.println((temp4/gas), DEC);
    
    //Serial.println(results_2);
    //Serial.println(results_4);
    //Serial.println(results_5);
    //Serial.println(gas, DEC);
    //Serial.println((temp1/gas), DEC);
    //Serial.println((temp2/gas), DEC);
    //Serial.println((temp3/gas), DEC);
    //Serial.println((temp4/gas), DEC);
    //Serial.println(valore1, DEC);
    //Serial.println(valore2, DEC);
    //Serial.println(valore3, DEC);
    //Serial.println(valore4, DEC);
    //Serial.println(results_6);
    //Serial.println(results_7);
    Serial.print("X: ");
    Serial.print(PosServo_X);
    Serial.print("     ");
    Serial.print("Y: ");
    Serial.println(PosServo_Y);

    Serial.print("X: ");
    Serial.print(iniz_GiroX);
    Serial.print("     ");
    Serial.print("Y: ");
    Serial.println(iniz_GiroY);
    
    //Serial.print("Z: ");
    //Serial.println(AcZ);
    delay(DEBUG_DELAY_TIME);
  #endif;
  
  
; }


