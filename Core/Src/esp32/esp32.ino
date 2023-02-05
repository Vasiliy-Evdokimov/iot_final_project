#define RXD2 16
#define TXD2 17

char tx[4] = {0};
char rx[4] = {0};

void setup() {  
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //
  delay(500);
  Serial.println("Setup!");
}

void loop() {
  //Choose Serial1 or Serial2 as required
  while (Serial2.available()) {
    Serial2.readBytes(rx, 4);
    Serial.println(rx[0], DEC);
    Serial.println(rx[1], DEC);
    Serial.println(rx[2], DEC);
    Serial.println(rx[3], DEC);
    //
    for (int i = 0; i < 4; i++)
      tx[i] = rx[i] + 1;
    //
    Serial2.flush();    
    Serial2.write(tx, 4);
    Serial.println("Sent!");  
  } 
  //
  Serial.println("Status!");
  delay(500);
}

/*
#include <HardwareSerial.h>
HardwareSerial SerialPort(2);

void setup()  
{
  Serial.begin(115200);
  SerialPort.begin(9600, SERIAL_8N1, RXD2, TXD2);   
  SerialPort.flush();
  //
  delay(500);
  Serial.println("Setup!");
} 

void loop()  
{  
  while (SerialPort.available()) {
    SerialPort.readBytes(rx, 4);    
    Serial.println(rx[0], DEC);
    Serial.println(rx[1], DEC);
    Serial.println(rx[2], DEC);
    Serial.println(rx[3], DEC);    
    //
    for (int i = 0; i < 4; i++)
      tx[i] = rx[i] + 1;
    //
    //
    SerialPort.flush();
    SerialPort.write(tx, 4);
    Serial.println("Sent!");
  }  
  //
  Serial.println("Status!");
  delay(500);
}
*/
