#include "Arduino.h"

#include "utils.hpp"
#include "web_module.hpp"
#include "uart_module.hpp"
#include "mqtt_module.hpp"
#include "html_page.hpp"
#include "auth.hpp"

WebServer server(80);

void web_handle() 
{
  server.handleClient();
}

void wifi_init() 
{
  con_println("");
  con_print("Connecting to ");
  con_println(ssid);
  //
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    con_print(".");
  }
  //
  con_println("");
  con_println("WiFi connected!");
  con_println("IP address: ");
  Serial.println(WiFi.localIP());
}

void web_init() 
{
  server.on("/", handleRoot);
  server.on("/getStatus", handleGetStatus);
  server.on("/setMode", handleSetMode);
  server.on("/setAlerts", handleSetAlerts);
  server.on("/setDevices", handleSetDevices);
  server.on("/setPlcMasks", handleSetPlcMasks);
  server.on("/delMqttPath", handleDelMqttPath);
  server.on("/addMqttPath", handleAddMqttPath);
  //
  server.begin();
  //
  con_println("HTTP server started!");
}

void handleRoot()
{  
  con_println("handleRoot()");
  //
  uart_complete_status();
  //
  server.send(200, "text/html", main_page);
}

void handleGetStatus()
{
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
  json += ", \"plc_outputs_masks\": [";
  for (int i = 0; i < PLC_OUTPUTS_COUNT; i++) {
    json += String("{") +
      "\"index\": " + String(i) +  ", " +
      "\"mask\": " + String(plc_outputs_masks[i].mask) +  ", " +
      "\"all_bits\": " + String(plc_outputs_masks[i].all_bits) +
    "}";
    json += (i < PLC_OUTPUTS_COUNT - 1) ? ", " : "";
  }  
  json += "]";
  //
  json += ", \"plc_inputs_states\": " + String(plc_inputs_states);
  json += ", \"plc_outputs_states\": " + String(plc_outputs_states);
  //
  //
  json += ", \"mqtt_subscribe_topics\": [";
  for (int i = 0; i < MQTT_MAX_SUBSCRIBE_TOPICS; i++) {
    json += String("{") +
      "\"index\": " + String(i) +  ", " +
      "\"path\": \"" + String(mqtt_subscribe_topics[i].path) + "\", " + 
      "\"value\": \"" + String(mqtt_subscribe_topics[i].value) + "\"" + 
    "}";
    json += (i < MQTT_MAX_SUBSCRIBE_TOPICS - 1) ? ", " : "";
  }  
  json += "]";
  //
  json += "}";
  //  
  //con_println("json = " + json);
  //
  server.send(200, "text/plane", json);
}

void handleSetMode()
{
  con_println("handleSetMode()");
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
  con_println("modeID=" + String(modeID) + " param=" + String(param));
  server.send(200, "text/plane", 0);
}

void handleSetAlerts()
{
  con_println("handleSetAlerts()");
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

void handleSetDevices()
{
  con_println("handleSetDevices()");
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
    con_println(s + "=" + a);
  }
  tx[1] = i;
  fillTxCRC(tx);
  //
  uart_transmit();
  //
  server.send(200, "text/plane", 0);
}

void handleSetPlcMasks()
{
  con_println("handleSetPlcMasks()");
  //
  String s, a;
  memset(tx, 0, BUFFER_SIZE);
  tx[0] = CMD_SET_PLC_MASKS;
  uint8_t i = 2;
  for (int j = 0; j < PLC_OUTPUTS_COUNT; j++) {
    s = "plc_output_" + String(j + 1) + "_mask";
    a = server.arg(s);    
    tx[i++] = a.toInt();
    con_println(s + "=" + a);
    //
    s = "plc_output_" + String(j + 1) + "_all_bits";
    a = server.arg(s);    
    tx[i++] = a.toInt();
    con_println(s + "=" + a);
  }
  tx[1] = i;
  fillTxCRC(tx);
  //
  uart_transmit();
  //
  server.send(200, "text/plane", 0);
}

void handleDelMqttPath()
{
  con_println("handleDelMqttPath()");
  //
  uint8_t path_id = server.arg("path_id").toInt();
  mqtt_unsubscribe_from_topic(path_id);  
  //
  server.send(200, "text/plane", 0);
}

void handleAddMqttPath() 
{
  con_println("handleAddMqttPath()");
  //
  char buf[MQTT_MAX_VALUE_LENGTH];
  server.arg("new_path").toCharArray(buf, MQTT_MAX_VALUE_LENGTH);
  printf("%s", buf);
  //
  server.send(200, "text/plane", 0);
}

