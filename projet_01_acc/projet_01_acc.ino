
#include <Wire.h>
#include <Servo.h>
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
int xx, yy, zz, servoX;

// Serial in for motor
const int outPin = 6;
byte slipPacket[256];
// servo
Servo moteur;
int posMoteur = 0; // center


void setup() {
  // Serial
  Serial.begin(115200);
  pinMode(6, OUTPUT);
  // I2C
  Wire.begin();
  Wire.beginTransmission(SENSOR); // on prend les infos du sensor
  Wire.write(POWER_CTL); // on dit a quel register on veut communiquer
  //on met 8 pour acceder a D3 
  Wire.write(8); // le sensor s'allume
  Wire.endTransmission();
  delay(10);
  
  calibration();

  // servo init pin
  moteur.attach(9);
}
void calibration(){
  /*
  value - 256 = i
  correction = -round(i/4)
  */  
  // x
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSX);
  Wire.write(-5);
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
  Wire.write(64);
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
  
  //******** Test print**********
  //testPrint();
  //******** Send Serial **********
  sendToSerial();
 
  // control moteur
  int packetSize = 0;
  packetSize = SLIPSerialRead(slipPacket);
  packetSize = SLIPSerialRead( slipPacket );
  for (int i=0 ; i < packetSize; i++) {
    analogWrite(outPin, slipPacket[i]);
  }
  // control servo
  servoX = x1|x2<<8;
  //posMoteur = 0;
  posMoteur = map(servoX, -255, 255, 0, 180);
  moteur.write(posMoteur);
}

/*----------------------------------*/
void testPrint(){
  xx = (x1 | x2 << 8);
  yy = (y1 | y2 << 8);
  zz = (z1 | z2 << 8);
/*
  xx = xx/64;
  yy = yy/64;
  zz = zz/64;
  */
  Serial.print("x ");
  Serial.print(xx);
  Serial.print(" y ");
  Serial.print(yy);
  Serial.print(" z ");
  Serial.print(zz);
  Serial.print("\n");
  roll = atan(yy / sqrt(pow(xx, 2) + pow(zz, 2))) * 180 / PI;
  pitch = atan(-1 * xx / sqrt(pow(yy, 2) + pow(zz, 2))) * 180 / PI;
 /* Serial.print("roll ");
  Serial.print(roll);
  Serial.print(" pitch ");
  Serial.print(pitch);
  Serial.print("\n");*/
}

/*----------------------------------*/
void sendToSerial(){
  SLIPSerialWrite(x1);
  SLIPSerialWrite(x2);
  SLIPSerialWrite(y1);
  SLIPSerialWrite(y2);
  SLIPSerialWrite(z1);
  SLIPSerialWrite(z2);
  Serial.write(END);
  delay(2);
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

// pour moteur

int SLIPSerialRead(byte * slipPacket){
  int packetIndex = 0;
  boolean escape = false;
  boolean packetComplete = false;
  if(Serial.available() == 0){
    return 0;
  }
  while(!packetComplete){
    if(Serial.available()>0){
      byte b = Serial.read();
      if(escape){
        if(b == ESC_END){
          slipPacket[packetIndex] = END;
        }else if(b == ESC_ESC){
          slipPacket[packetIndex] = ESC;
        }
        packetIndex++;
        escape = 0;
      }else if(b == END){
        packetComplete = true;
      }else if(b == ESC){
        escape = 1;
      }else{
        slipPacket[packetIndex] = b;
        packetIndex++;
      }
    }
  }
  return packetIndex;
}
