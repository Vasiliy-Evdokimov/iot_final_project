#include "Arduino.h"

#include "uart_module.hpp"
#include "utils.hpp"
#include "mqtt_module.hpp"

uint8_t tx[BUFFER_SIZE] = {0};
uint8_t rx[BUFFER_SIZE] = {0};

extern sensor sensors[];
extern device_state devices_states[];
extern mode current_mode;

void con_print(String s) {
  Serial.print(s);
}

void con_println(String s) {
  Serial.println(s);
}

void uart_init() {
  Serial.begin(115200);
  Serial2.begin(UART_BAUDRATE, SERIAL_8N1, RXD2, TXD2);  
}

void uart_transmit() {
  if (!tx[0]) return;
  //
  Serial2.flush();
  Serial2.write(tx, BUFFER_SIZE);
}

void print_buffer(uint8_t* aBuf, String aPrefix) {
  for (int i = 0; i < aBuf[1]; i++) 
    Serial.println(aPrefix + "_" + String(i) + "=" + String(aBuf[i]));
}

void uart_complete_status() {
  memset(tx, 0, BUFFER_SIZE);
  tx[0] = CMD_GET_STATUS;
  tx[1] = 2;
  fillTxCRC(tx);
  //
  uart_transmit();
}

void uart_handle() {

  uint8_t rx0, rx1, idx;

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
      break;      
    }
    //
    print_buffer(rx, "RX");
    //
    if (rx0 == MSG_MODE) {
      current_mode.type = rx[2];
      current_mode.period = rx[3];
      current_mode.percents = rx[4];
      //
      publishMode();
    } else
    //
    if (rx0 == MSG_SENSORS) {
      sensor* psensor;
      //
      for (int i = 0; i < rx1 / 6; i++) {
        idx = 2 + i * 6;
        psensor = getSensorByType(rx[idx]);
        if (!psensor) continue;
        psensor->value = rx[idx + 1];        
        psensor->alert_check = rx[idx + 2];
        psensor->alert_compare = rx[idx + 3];
        psensor->alert_value = rx[idx + 4];
        psensor->alert_flag = rx[idx + 5];
      }     
      //
      publishSensors();
    } else
    //
    if (rx0 == MSG_DEVICES) {
      device_state* pdevice_state;
      //
      for (int i = 0; i < rx1 / 2; i++) {
        idx = 2 + i * 2;
        pdevice_state = getDeviceStateByType(rx[idx]);
        if (!pdevice_state) continue;
        pdevice_state->value = rx[idx + 1];
      }
      //
      publishDevices();
    }
        
  } 

}