#include "utils.c"
#include "auth.h"

#define RXD2 16
#define TXD2 17

uint8_t tx[BUFFER_SIZE] = {0};
uint8_t rx[BUFFER_SIZE] = {0};

void setup() {  
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //
  delay(500);
  Serial.println("Setup!");
}

void printRX() {
  for (int i = 0; i < rx[1]; i++) 
    Serial.println("RX_" + (String)i + "=" + (String)(rx[i]));
}

uint8_t rx0, rx1;

void loop() {
  //Choose Serial1 or Serial2 as required
  while (Serial2.available()) {
    Serial2.readBytes(rx, BUFFER_SIZE);
    rx0 = rx[0];
    rx1 = rx[1];
    //    
    uint8_t crc = getCRC(rx1, rx);
    //
    if (crc != rx[rx1]) {
      Serial.println("CRC failed!");
      //rx0 = 0;
    }
    //
    if (rx0 == MSG_SENSORS_DATA) {
      printRX();
    }    
  } 
  //
  Serial.println("Status!");
  delay(500);
}
