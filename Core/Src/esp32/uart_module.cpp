#include "Arduino.h"

#include "utils.hpp"
#include "uart_module.hpp"
#include "mqtt_module.hpp"

int rx_counter = 0;
int tx_counter = 0;

void con_print(String s) 
{
  Serial.print(s);
}

void con_println(String s) 
{
  Serial.println(s);
}

void uart_init() 
{
  Serial.begin(SERIAL_BAUDRATE);
  Serial2.begin(UART_BAUDRATE, SERIAL_8N1, RXD2, TXD2);  
}

void uart_transmit() 
{
  if (!tx[0]) return;
  //
  Serial2.flush();
  Serial2.write(tx, BUFFER_SIZE);
  //  
  tx_counter++;
  con_println("New pack sended (" + String(tx_counter) + ")");
  //  
  print_buffer(tx, "TX");
}

void print_buffer(uint8_t* aBuf, String aPrefix) 
{
  con_print(aPrefix + ":");
  for (int i = 0; i <= aBuf[1]; i++)
    con_print(" [" + String(i) + "]=" + String(aBuf[i]));
  con_println("");
}

void uart_complete_status() 
{
  memset(tx, 0, BUFFER_SIZE);
  tx[0] = CMD_GET_STATUS;
  tx[1] = 2;
  fillTxCRC(tx);
  //
  uart_transmit();
}

void uart_handle() 
{
  uint8_t rx0, rx1, idx;

  while (Serial2.available()) 
  {
    Serial2.readBytes(rx, BUFFER_SIZE);
    rx0 = rx[0];
    rx1 = rx[1];
    //    
    uint8_t crc = getCRC(rx1, rx);    
    //
    if (crc != rx[rx1]) {
      con_println("CRC failed!");
      //rx0 = 0;
      break;      
    }
    //
    rx_counter++;
    con_println("New pack received (" + String(rx_counter) + ")");
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
    } else
    //
    if (rx0 == MSG_PLC_MASKS) {
      plc_inputs_states = rx[2];
      plc_outputs_states = rx[3];
      //
      for (int i = 0; i < PLC_INPUTS_COUNT; i++)
      {
        plc_outputs_masks[i].mask = rx[4 + i * 2];
        plc_outputs_masks[i].all_bits = rx[4 + i * 2 + 1];
      }
      //      
      publishPLC();
    }
  } 
}