
#include <Wire.h>

const int SENSOR = 0x53; // adress du accelerometre
const int POWER_CTL = 0x2D; // adress du register
const int XAXIS = 0x32; // adress du data X-axis
const int OFSX = 0x1E; // valeur ofset pour calibration
const int OFSY = 0x1F;
const int OFSZ = 0x20;

float x, y, z; // variable output du sensor

void setup() {
  Serial.begin(9600);
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
}

void loop() {
  Wire.beginTransmission(SENSOR);
  Wire.write(XAXIS);
  Wire.endTransmission(false);
  Wire.requestFrom(SENSOR, 6, true); // on demande les valeurs des 6 axes a partir de xAxis
  
  x = (Wire.read() | Wire.read()<<8);
  //Serial.print("X= ");
  //Serial.print(x);
  x = x/256;
  y = (Wire.read() | Wire.read()<<8);
  //Serial.print("y= ");
  //Serial.print(y);
  y = y/256;
  z = (Wire.read() | Wire.read()<<8);
  //Serial.print("z= ");
  //Serial.print(z);
  z = z/256;
 // Serial.print("\n");
  
  Serial.print("X= ");
  Serial.print(x);
  Serial.print(" Y= ");
  Serial.print(y);
  Serial.print(" Z= ");
  Serial.print(z);
  Serial.print("\n");
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