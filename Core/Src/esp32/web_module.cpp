#include "Arduino.h"

#include "utils.hpp"
#include "web_module.hpp"
#include "uart_module.hpp"
#include "html_page.hpp"
#include "auth.hpp"

WebServer server(80);

void web_handle() {
  server.handleClient();
}

void wifi_init() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

void web_init() {  
  server.on("/", handleRoot);
  server.on("/getStatus", handleGetStatus);
  server.on("/setMode", handleSetMode);
  server.on("/setAlerts", handleSetAlerts);
  server.on("/setDevices", handleSetDevices);
  //
  server.begin();
  //
  Serial.println("HTTP server started!");  
}

void handleRoot() {  
  Serial.println("handleRoot()");
  //
  uart_complete_status();
  //
  server.send(200, "text/html", main_page);
}

void handleGetStatus() {
  String json = "{"; 
  //  
  json += String("\"mode\": {") + 
    "\"type\": " + String(current_mode.type) + ", " +  
    "\"period\": " + String(current_mode.period) + ", " + 
    "\"percent\": " + String(current_mode.percents) +
  "}";  
  //
  json += ", \"sensors\": [";  
  for (int i = 0; i < SENSORS_COUNT; i++) {
      json += "{\"name\": \"" + String(sensor_names[i]) +  "\", " + 
        "\"data\": {" + 
          "\"value\": " + String(sensors[i].value) + ", " +
          "\"alert_check\": " + String(sensors[i].alert_check) + ", " +
          "\"alert_compare\": " + String(sensors[i].alert_compare) +  ", " +
          "\"alert_value\": " + String(sensors[i].alert_value) +  ", " +          
          "\"alert_flag\": " + String(sensors[i].alert_flag) + 
        "}}";
      json += (i < SENSORS_COUNT - 1) ? ", " : "";         
  }
  json += "]";
  //  
  json += ", \"devices\": [";  
  for (int i = 0; i < DEVICES_COUNT; i++) {
    json += String("{") +
      "\"name\": \"" + String(device_names[i]) +  "\", " + 
      "\"value\": " + String(devices_states[i].value) + 
    "}";
    json += (i < DEVICES_COUNT - 1) ? ", " : "";
  }  
  json += "]";  
  //
  json += "}";
  //  
  //Serial.println("json = " + json);
  //
  server.send(200, "text/plane", json);
}

void handleSetMode() {
  Serial.println("handleSetMode()");
  uint8_t modeID = server.arg("mode").toInt();
  uint8_t param = 0;
  //
  if (modeID == MODE_PERIODIC) 
    param = server.arg("check_period").toInt();
  if (modeID == MODE_IFCHANGED) 
    param = server.arg("check_percents").toInt();
  //
  memset(tx, 0, BUFFER_SIZE);
  tx[0] = CMD_SET_MODE;
  tx[1] = 4;
  tx[2] = modeID;
  tx[3] = param;
  fillTxCRC(tx);
  //
  uart_transmit();
  //
  Serial.println("modeID=" + String(modeID) + " param=" + String(param));
  server.send(200, "text/plane", 0);
}

void handleSetAlerts() {
  Serial.println("handleSetAlerts()");    
  //  
  String s;
  memset(tx, 0, BUFFER_SIZE);
  tx[0] = CMD_SET_ALERTS;
  uint8_t i = 2;
  for (int j = 0; j < SENSORS_COUNT; j++) {    
    s = String(sensor_names[j]);
    tx[i++] = sensors[j].type;
    tx[i++] = server.arg(s + "_alert_check").toInt();
    tx[i++] = server.arg(s + "_alert_compare").toInt();
    tx[i++] = server.arg(s + "_alert_value").toInt();
  }
  tx[1] = i;
  fillTxCRC(tx);
  //
  print_buffer(tx, "TX");
  //
  uart_transmit();
  //
  server.send(200, "text/plane", 0);
}

void handleSetDevices() {
  Serial.println("handleSetDevices()");    
  //
  String s, a;
  memset(tx, 0, BUFFER_SIZE);
  tx[0] = CMD_SET_DEVICES;
  uint8_t i = 2;
  for (int j = 0; j < DEVICES_COUNT; j++) {
    s = String(device_names[j]) + "_switch";
    a = server.arg(s);    
    tx[i++] = j + 1;
    tx[i++] = a.toInt();    
    Serial.println(s + "=" + a);
  }
  tx[1] = i;
  fillTxCRC(tx);
  //
  uart_transmit();
  //
  server.send(200, "text/plane", 0);
}