
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

char x, y, z;
float roll, pitch;
int servoX;

// Serial in for motor
const int outPin = 6;
byte slipPacket[256];
// servo
Servo moteur;
int posMoteur = 0; // center


void setup() {
  // Serial
  Serial.begin(9600);
  pinMode(6, OUTPUT);
  // I2C
  Wire.begin();
  Wire.beginTransmission(SENSOR); // on prend les infos du sensor
  Wire.write(POWER_CTL); // on dit a quel register on veut communiquer
  //on met 8 pour acceder a D3 
  Wire.write(8); // le sensor s'allume
  Wire.endTransmission();
  delay(10);

 
  // calibration
    // x 
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSX);
  Wire.write(20);
  Wire.endTransmission();
  delay(10);
    // y
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSY);
  Wire.write(81);
  Wire.endTransmission();
  delay(10);
    // z
  Wire.beginTransmission(SENSOR);
  Wire.write(OFSZ);
  Wire.write(8);
  Wire.endTransmission();
  delay(37);

  // servo init pin
  moteur.attach(9);
}

void loop() {
  Wire.beginTransmission(SENSOR);
  Wire.write(XAXIS);
  Wire.endTransmission(false);
  // on demande les valeurs des 6 axes a partir de xAxis
  Wire.requestFrom(SENSOR, 6, true); 
  // i2c : MSB first
  // output en two's complement

  x = (Wire.read() | Wire.read()<<8);
  y = (Wire.read() | Wire.read()<<8);
  z = (Wire.read() | Wire.read()<<8);

  roll = atan(y / sqrt(pow(x, 2) + pow(z, 2))) * 180 / PI;
  pitch = atan(-1 * x / sqrt(pow(y, 2) + pow(z, 2))) * 180 / PI;
  //******** Test print**********
  //testPrint();
  //******** Send Serial **********
  sendToSerial();

  int posi = (x-100)*5;
  posMoteur = map(posi, -255, 255, 0, 180);
  moteur.write(posMoteur);
}

/*----------------------------------*/
void testPrint(){

  float ax = x/256;
  float ay = y/256;
  float az = z/256;
  
  Serial.print("x ");
  Serial.print(ax);
  Serial.print(" y ");
  Serial.print(ay);
  Serial.print(" z ");
  Serial.print(az);
  Serial.print("\n");
}

/*----------------------------------*/
void sendToSerial(){
  // x
  SLIPSerialWrite(x>>8);
  SLIPSerialWrite(x&255);
  // y
  SLIPSerialWrite(y>>8);
  SLIPSerialWrite(y&255);
  // z
  SLIPSerialWrite(z>>8);
  SLIPSerialWrite(z&255);

  Serial.write(END);
  delay(3);
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
