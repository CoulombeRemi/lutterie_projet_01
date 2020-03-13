
#include <Wire.h>
const int END = 192;
const int ESC = 219; 
const int ESC_END = 220;
const int ESC_ESC = 221;

const int SENSOR = 0x53; // adress du sensor
const int POWER_CTL = 0x2D; // adress du register
const int XAXIS = 0x32; // adress du data X-axis
const int OFSX = 0x1E; // valeur ofset pour calibration
const int OFSY = 0x1F;
const int OFSZ = 0x20;

float x_f, y_f, z_f; // variable output du sensor
int x_i_msb, y_i_msb, z_i_msb, x_i_lsb, y_i_lsb, z_i_lsb;
char x1, x2, y1, y2, z1, z2;
float roll, pitch;

int xx, yy, zz;

//******Moteur***
byte slipPacket[256];
int speed; // 0- 255


void calibration(){  
  // x
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSX);
  Wire.write(64);
  Wire.endTransmission();
  delay(10);
  // y
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSY);
  Wire.write(69);
  Wire.endTransmission();
  delay(10);
  // z
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSZ);
  Wire.write(4);
  Wire.endTransmission();
  delay(10);
}

void setup() {
  // Serial
  Serial.begin(115200);
  pinMode(6, OUTPUT);
  // I2C
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
  
  /******************************/

    // x
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSX);
  Wire.write(64);
  Wire.endTransmission();
  delay(10);
  // y
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSY);
  Wire.write(69);
  Wire.endTransmission();
  delay(10);
  // z
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSZ);
  Wire.write(4);
  Wire.endTransmission();
  delay(10);
}

void loop() {
  Wire.beginTransmission(SENSOR);
  Wire.write(XAXIS);
  Wire.endTransmission(false);
  // on demande les valeurs des 6 axes a partir de xAxis
  Wire.requestFrom(SENSOR, 6, true); 
  // i2c : MSB first
  // output en two's complement
  x1 = Wire.read();
  x2 = Wire.read();
  y1 = Wire.read();
  y2 = Wire.read();
  z1 = Wire.read();
  z2 = Wire.read();
//******************
/*  xx = (x1 | x2 << 8);
  yy = (y1 | y2 << 8);
  zz = (z1 | z2 << 8);


  xx = Wire.read() | Wire.read()<<8;
  yy = Wire.read() | Wire.read()<<8;
  zz = Wire.read() | Wire.read()<<8;
  // divise par 64 pour plus de presision
  xx = xx/64;
  yy = yy/64;
  zz = zz/64;
  

  roll = atan(yy / sqrt(pow(xx, 2) + pow(zz, 2))) * 180 / PI;
  pitch = atan(-1 * xx / sqrt(pow(yy, 2) + pow(zz, 2))) * 180 / PI;
  
  Serial.print("roll ");
  Serial.print(roll);
  Serial.print(" pitch ");  
  Serial.print(pitch);
  Serial.print("\n");

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
  int packetSize = 0;
  int i;
  packetSize = SLIPSerialRead(slipPacket);

  // control moteur
  analogWrite(6, slipPacket);
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
