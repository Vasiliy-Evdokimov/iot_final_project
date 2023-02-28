#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include <PubSubClient.h>

#include "utils.c"
#include "auth.h"
#include "html_page.h"

#define UART_BAUDRATE 9600
#define RXD2 16
#define TXD2 17

WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

const char* sensors_settings[] = {
  "mode",
  "period",
  "percents"
};

const char* sensor_params[] = {
  "value",
  "alert_check",
  "alert_compare",
  "alert_value",
  "alert_flag"
};

const char* sensor_names[] = {
  "temperature",
  "humidity",
  "ambient"
};

const char* device_params[] = {
  "state"
};

const char* device_names[] = {
  "led_red",
  "led_blue"
};

uint8_t tx[BUFFER_SIZE] = {0};
uint8_t rx[BUFFER_SIZE] = {0};

char mqtt_topic[128];
char mqtt_value[20];

extern sensor sensors[SENSORS_COUNT];
extern device_state devices_states[DEVICES_COUNT];
extern mode current_mode;

void DoUartTransmit() {
  if (!tx[0]) return;
  Serial2.flush();
  Serial2.write(tx, BUFFER_SIZE);
}

void handleRoot() {  
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
  DoUartTransmit();
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
  DoUartTransmit();
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
  DoUartTransmit();
  //
  server.send(200, "text/plane", 0);
}

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
  //
  char buf[20] = {0};
  Serial.print("length = " + String(length) + "; payload = ");
  for (int i = 0; i < length; i++)
    buf[i] = (char)payload[i];
  Serial.println(buf);
}

void mqttconnect() {
  while (!client.connected()) {
    Serial.print("MQTT connecting... ");
    String clientId = MQTT_CLIENT_ID;
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected!");
      topicsSubscribe();
    } else {
      Serial.print("Failed, status code = ");
      Serial.print(client.state());
      Serial.println(" Try again in 5 seconds...");
      delay(5000);
    }
  }
}

void topicsInit() {
  snprintf(mqtt_topic, 128, "%s/sensors/settings/mode", MQTT_ROOT);
  snprintf(mqtt_value, 20, "%d", MODE_PERIODIC);
  client.publish(mqtt_topic, mqtt_value);
  snprintf(mqtt_topic, 128, "%s/sensors/settings/period", MQTT_ROOT);
  snprintf(mqtt_value, 20, "%d", 5);
  client.publish(mqtt_topic, mqtt_value);
  //
  for (int i = 0; i < DEVICES_COUNT; i++)
    for (int j = 0; j < 1; j++) {
      snprintf(mqtt_topic, 128, "%s/devices/%s/%s", MQTT_ROOT, 
        device_names[i], device_params[j]);
      client.publish(mqtt_topic, "0");    
    }           
  //
  for (int i = 0; i < SENSORS_COUNT; i++) 
    for (int j = 0; j < 4; j++) {
      snprintf(mqtt_topic, 128, "%s/sensors/%s/%s", MQTT_ROOT, 
        sensor_names[i], sensor_params[j]);      
      client.publish(mqtt_topic, "0");   
    }   
}

void topicsSubscribe() {
  for (int i = 0; i < 2; i++) {
    snprintf(mqtt_topic, 128, "%s/sensors/settings/%s", MQTT_ROOT, sensors_settings[i]);
    client.subscribe(mqtt_topic);
  }
  //
  for (int i = 0; i < DEVICES_COUNT; i++) {
    snprintf(mqtt_topic, 128, "%s/devices/%s/state", MQTT_ROOT, device_names[i]);
    client.subscribe(mqtt_topic);
  }          
  //
  for (int i = 0; i < SENSORS_COUNT; i++) {
    snprintf(mqtt_topic, 128, "%s/sensors/%s/check_alert", MQTT_ROOT, sensor_names[i]);
    client.subscribe(mqtt_topic);
    snprintf(mqtt_topic, 128, "%s/sensors/%s/alert_value", MQTT_ROOT, sensor_names[i]);
    client.subscribe(mqtt_topic);    
  }          
}

void initWebServer() {  
  server.on("/", handleRoot);
  server.on("/getStatus", handleGetStatus);
  server.on("/setMode", handleSetMode);
  server.on("/setAlerts", handleSetAlerts);
  server.on("/setDevices", handleSetDevices);
}

void setup() {  
  Serial.begin(115200);
  Serial2.begin(UART_BAUDRATE, SERIAL_8N1, RXD2, TXD2);
  //
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
  //  
  client.setServer(mqtt_server, mqtt_port);  
  //    
  mqttconnect();
  topicsInit();  
  topicsSubscribe();  
  //
  client.setCallback(receivedCallback);
  //
  initWebServer();
  server.begin();
  Serial.println("HTTP server started!");
  //
  initSensors();
  initDevicesStates();
  initMode();
  //
  delay(500);
  Serial.println("Setup!");      
}

void printRX() {
  for (int i = 0; i < rx[1]; i++) 
    Serial.println("RX_" + String(i) + "=" + String(rx[i]));
}

uint8_t rx0, rx1, idx;
sensor* psensor; 
device_state* pdevice_state; 

void loop() {  
  
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
    printRX();
    //
    if (rx0 == MSG_MODE) {
      current_mode.type = rx[2];
      current_mode.period = rx[3];
      current_mode.percents = rx[4];
    } else
    //
    if (rx0 == MSG_SENSORS) {
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
      for (int i = 0; i < SENSORS_COUNT; i++) {
        snprintf(mqtt_topic, 128, "%s/sensors/%s/value", MQTT_ROOT, sensor_names[i]);
        snprintf(mqtt_value, 20, "%d", rx[i * 2 + 3]);
        client.publish(mqtt_topic, mqtt_value);
        snprintf(mqtt_topic, 128, "%s/sensors/%s/alert_flag", MQTT_ROOT, sensor_names[i]);
        snprintf(mqtt_value, 20, "%d", 0);
        client.publish(mqtt_topic, mqtt_value);
        delay(100);
      }     
      Serial.println("Published!");      
    } else
    //
    if (rx0 == MSG_DEVICES) {
       for (int i = 0; i < rx1 / 2; i++) {
        idx = 2 + i * 2;
        pdevice_state = getDeviceStateByType(rx[idx]);
        if (!pdevice_state) continue;
        pdevice_state->value = rx[idx + 1];
      }
    }
        
  } 
  //  
  if (!client.connected()) {
    mqttconnect();
    topicsSubscribe();
  }  
  //
  client.loop();
  //
  server.handleClient();
  delay(1);  
  
}
