
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

float x_f, y_f, z_f; // variable output du sensor
int x_i_msb, y_i_msb, z_i_msb, x_i_lsb, y_i_lsb, z_i_lsb;
char x1, x2, y1, y2, z1, z2;

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

  //calibration();
}

void loop() {
  Wire.beginTransmission(SENSOR);
  Wire.write(XAXIS);
 // Wire.endTransmission(false);
  Wire.endTransmission(false);
  Wire.requestFrom(SENSOR, 6, true); // on demande les valeurs des 6 axes a partir de xAxis
  // Resolution de 10bit --> 2oct
  /*x_f = (Wire.read() | Wire.read()<<8);
  x_f = x_f/256;
  y_f = (Wire.read() | Wire.read()<<8);
  y_f = y_f/256;
  z_f = (Wire.read() | Wire.read()<<8);
  z_f = z_f/256;
*/
  x1 = Wire.read();
  x2 = Wire.read();
  y1 = Wire.read();
  y2 = Wire.read();
  z1 = Wire.read();
  z2 = Wire.read();

  /*
  x1 /= 256;
  x2 /= 256;
  y1 /= 256;
  y2 /= 256;
  z1 /= 256;
  z2 /= 256;
*//*
  Serial.print("X= ");
  Serial.print(x1);
  Serial.print(" Y= ");
  Serial.print(y1);
  Serial.print(" Z= ");
  Serial.print(z1);
  Serial.print("\n");

  if(x1 > 255){
    Serial.print("\n\n HELP! \n\n");
  }
  */
  // conversion en int
 /*x_i_msb = static_cast<int>(x1);
  x_i_lsb = static_cast<int>(x2);
  y_i_msb = static_cast<int>(y1);
  y_i_lsb = static_cast<int>(y2);
  z_i_msb = static_cast<int>(z1);
  z_i_lsb = static_cast<int>(z2);
*/
  // envoie au serial port
  SLIPSerialWrite(x1);
  SLIPSerialWrite(x2);
  SLIPSerialWrite(y1);
  SLIPSerialWrite(y2);
  SLIPSerialWrite(z1);
  SLIPSerialWrite(z2);

  Serial.write(END);
  delay(2);
}

void sendToSerial(int x, int y, int z){
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
