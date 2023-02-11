#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include <PubSubClient.h>

#include "utils.c"
#include "auth.h"

#define UART_BAUDRATE 9600
#define RXD2 16
#define TXD2 17

WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

const char * sensors_settings[] {
  "mode",
  "period"  
};

const char * sensor_params[] = {
  "value",
  "alert_flag",
  "check_alert",
  "alert_value"
};

const char * sensor_names[] = {
  "temperature",
  "humidity",
  "ambient"
};

const char * device_params[] = {
  "state"
};

const char * device_names[] = {
  "led",
  "fan"
};

uint8_t tx[BUFFER_SIZE] = {0};
uint8_t rx[BUFFER_SIZE] = {0};

char mqtt_topic[128];
char mqtt_value[20];

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
  //
  char buf[20] = {0};
  Serial.print("length = " + (String)length + "; payload = ");
  for (int i = 0; i < length; i++)
    buf[i] = (char)payload[i];
  Serial.println(buf);
}

void mqttconnect() {
  while (!client.connected()) {
    Serial.print("MQTT connecting... ");
    String clientId = "Evdokimov_VI_ESP32";
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
  delay(500);
  Serial.println("Setup!");      
}

void printRX() {
  for (int i = 0; i < rx[1]; i++) 
    Serial.println("RX_" + (String)i + "=" + (String)(rx[i]));
}

uint8_t rx0, rx1;
sensor* s; 

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
    if (rx0 == MSG_SENSORS_DATA) {
      printRX();
      //
      for (int i = 0; i < SENSORS_COUNT; i++) {
        snprintf(mqtt_topic, 128, "%s/sensors/%s/value", MQTT_ROOT, sensor_names[i]);
        snprintf(mqtt_value, 20, "%d", rx[i*2 + 3]);
        client.publish(mqtt_topic, mqtt_value);
        snprintf(mqtt_topic, 128, "%s/sensors/%s/alert_flag", MQTT_ROOT, sensor_names[i]);
        snprintf(mqtt_value, 20, "%d", 0);
        client.publish(mqtt_topic, mqtt_value);
        delay(100);
      }     
      Serial.println("Published!");
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
  Serial.println("Status!");
  delay(500);
  
}
