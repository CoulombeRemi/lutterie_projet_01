#include <Thread.h>


const byte END=192;
const byte ESC=219; 
const byte ESC_END=220;
const byte ESC_ESC=221;

const int outPin = 6;
byte slipPacket[256];

Thread sensorReadingThread = Thread();

void setup() {
  pinMode(outPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int packetSize = 0;
  packetSize = SLIPSerialRead( slipPacket );
  for (int i=0 ; i < packetSize; i++) {
    analogWrite(outPin, slipPacket[i]);
  }
}

void SLIPSerialWrite(byte value){
  if(value == END) { // Si c'est la valeur 192, on remplace par 219 et 220.
    Serial.write(ESC);
    Serial.write(ESC_END);
    return;
  } else if (value == ESC) {  // Si c'est la valeur 219, on fait suivre un 221.
    Serial.write(ESC);
    Serial.write(ESC_ESC);
    return;
  } else { // On envoie toutes les autres valeurs normalement.
    Serial.write(value);
    return;
  }
}

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
