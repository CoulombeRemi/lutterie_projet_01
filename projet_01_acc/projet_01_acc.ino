
#include <Wire.h>
const int END=192;
const int ESC=219; 
const int ESC_END=220;
const int ESC_ESC=221;

const int SENSOR = 0x53; // adress du accelerometre
const int POWER_CTL = 0x2D; // adress du register
const int XAXIS = 0x32; // adress du data X-axis
const int OFSX = 0x1E; // valeur ofset pour calibration
const int OFSY = 0x1F;
const int OFSZ = 0x20;
byte values[6]; // 6 valeurs car on a 2x les datas pour les axes
char serialOutput[512];

float x_f, y_f, z_f; // variable output du sensor
int x_i, y_i, z_i;
float x1, x2, y1, y2, z1, z2;

int xx, xx2, xx3;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(SENSOR); // on prend les infos du sensor
  Wire.write(POWER_CTL); // on dit a quel register on veut communiquer
  /*
  on met 8 pour acceder a D3 
  8 ==  0  0  0  0  1  0  0  0 (binaire)
        D7 D6 D5 D4 D3 D2 D1 D0 
   1 = enable
   0 = disable
  */
  Wire.write(8); // le sensor s'allume
  Wire.endTransmission();
  delay(10);

  calibration();
  //Serial.write(255);
}

void loop() {
  Wire.beginTransmission(SENSOR);
  Wire.write(XAXIS);
 // Wire.endTransmission(false);
  Wire.endTransmission(false);
  Wire.requestFrom(SENSOR, 6, true); // on demande les valeurs des 6 axes a partir de xAxis
  //Wire.requestFrom(SENSOR, 2, true);
  // Resolution de 10bit --> 2oct
  //Serial.write(Wire.read());
  /*x_f = (Wire.read() | Wire.read()<<8);
  x_f = x_f/256;
  y_f = (Wire.read() | Wire.read()<<8);
  y_f = y_f/256;
  z_f = (Wire.read() | Wire.read()<<8);
  z_f = z_f/256;
*/
x1 = Wire.read();
x2 = Wire.read()<<8;
y1 = Wire.read();
y2 = Wire.read()<<8;
z1 = Wire.read();
z2 = Wire.read()<<8;
  //xx = Wire.read(); // 1 octet
  //xx2 = Wire.read(); // 1 deuxieme octet
  //xx3 = xx | xx2<<8;
/*
  Serial.print("xx= ");
  Serial.print(xx);
  Serial.print("    xx2= ");
  Serial.print(xx2);
  Serial.print("    xx3= ");
  Serial.print(xx3);
  Serial.print("\n");
*/
  //SLIPSerialWrite(xx);
  //SLIPSerialWrite(xx2);
  // conversion des donnees en int
  /*x_i = static_cast<int>(x_f);
  y_i = static_cast<int>(y_f);
  z_i = static_cast<int>(z_f);
 
  Serial.print("X= ");
  Serial.print(x_i);
  Serial.print(" Y= ");
  Serial.print(y_i);
  Serial.print(" Z= ");
  Serial.print(z_i);
  Serial.print("\n");
*/

  x_i = static_cast<int>(x1);
  y_i = static_cast<int>(y1);
  z_i = static_cast<int>(z1);
 /* Serial.write(x_i);
  Serial.write(y_i);
  Serial.write(z_i);*/

  SLIPSerialWrite(x_i);
  SLIPSerialWrite(y_i);
  SLIPSerialWrite(z_i);

  Serial.write(END);
  delay(2);
}

void sendToSerial(int x, int y, int z){
  //Serial.write(x);
}

void calibration(){  
  // x
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSX);
  /*
  z = raw data(valeur sans etre divise par 256) - 256
  correction = -Round(z/4);
  */
  //Wire.write(correction);
  Wire.endTransmission();
  delay(10);
  // y
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSY);
  //Wire.write(64);
  Wire.endTransmission();
  delay(10);
  // z
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSZ);
  //Wire.write(13);
  Wire.endTransmission();
  delay(10);
}
void SLIPSerialWrite(int value){
  if(value == END){ 
    Serial.write(ESC);
    Serial.write(ESC_END);
    return;
  } else if(value == ESC) {  
    Serial.write(ESC);
    Serial.write(ESC_ESC);
    return;
  } else {
    Serial.write(value);
    return;
  }
}
