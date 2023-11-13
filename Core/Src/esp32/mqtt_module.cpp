#include "Arduino.h"

#include "mqtt_module.hpp"
#include "auth.hpp"
#include "certs.hpp"
#include "utils.hpp"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure wifi_client;
WiFiClientSecure wifi_client_sub;
PubSubClient mqtt_client;
PubSubClient mqtt_client_sub;

char mqtt_topic[128];
char mqtt_value[20];

extern mode current_mode;

extern const char* sensor_names[];
extern const char* device_names[];

void mqtt_init() {
  //if (!use_mqtt) return;
  //
  while (!(mqtt_client.connected() && 
           mqtt_client_sub.connected())) 
  { 
    wifi_client.setCACert(rootCA);
    wifi_client.setCertificate(cert_devices);
    wifi_client.setPrivateKey(key_devices);
    //
    wifi_client_sub.setCACert(rootCA);
    wifi_client_sub.setCertificate(cert_registries);
    wifi_client_sub.setPrivateKey(key_registries);
    //
    mqtt_client.setClient(wifi_client);
    mqtt_client.setServer(mqtt_server, mqtt_port);
    //
    mqtt_client_sub.setClient(wifi_client_sub);
    mqtt_client_sub.setServer(mqtt_server, mqtt_port);
    //
    mqtt_client_sub.setCallback(mqtt_callback);        
    //
    String clientId = MQTT_CLIENT_ID;
    //
    Serial.print("MQTT connecting... ");    
    if (mqtt_client.connect(clientId.c_str())) {
      Serial.println("MQTT connected!");
    } else {
      Serial.print("Failed, status code = ");
      Serial.print(mqtt_client.state());
      Serial.println(" Try again in 5 seconds...");
      delay(5000);
    }
    //
    Serial.print("MQTT_sub connecting... ");
    if (mqtt_client_sub.connect(clientId.c_str())) {
      Serial.println("MQTT_sub connected!");
      //
      mqtt_client_sub.subscribe(MQTT_SUBSCRIBE_PATH);
    } else {
      Serial.print("Failed, status code = ");
      Serial.print(mqtt_client_sub.state());
      Serial.println(" Try again in 5 seconds...");
      delay(5000);
    }
  }
}

void mqtt_callback(char* topic, byte *payload, unsigned int length) {  
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();  
}

void publishMode() {
  snprintf(mqtt_topic, 128, "%s/mode/type", MQTT_ROOT);
  snprintf(mqtt_value, 20, "%d", current_mode.type);
  mqtt_client.publish(mqtt_topic, mqtt_value);
  snprintf(mqtt_topic, 128, "%s/mode/period", MQTT_ROOT);
  snprintf(mqtt_value, 20, "%d", current_mode.period);
  mqtt_client.publish(mqtt_topic, mqtt_value);
  snprintf(mqtt_topic, 128, "%s/mode/percents", MQTT_ROOT);
  snprintf(mqtt_value, 20, "%d", current_mode.percents);
  mqtt_client.publish(mqtt_topic, mqtt_value);
  //
  Serial.println("Mode has been published!");
}

void publishSensors() {
  sensor* psensor;
  //
  for (int i = 0; i < SENSORS_COUNT; i++) {
    psensor = getSensorByType(i + 1);      
    if (!psensor) continue;
    //
    snprintf(mqtt_topic, 128, "%s/sensors/%s/value", MQTT_ROOT, sensor_names[i]);
    snprintf(mqtt_value, 20, "%d", psensor->value);
    mqtt_client.publish(mqtt_topic, mqtt_value);
    snprintf(mqtt_topic, 128, "%s/sensors/%s/alert_check", MQTT_ROOT, sensor_names[i]);
    snprintf(mqtt_value, 20, "%d", psensor->alert_check);
    mqtt_client.publish(mqtt_topic, mqtt_value);
    snprintf(mqtt_topic, 128, "%s/sensors/%s/alert_compare", MQTT_ROOT, sensor_names[i]);
    snprintf(mqtt_value, 20, "%d", psensor->alert_compare);
    mqtt_client.publish(mqtt_topic, mqtt_value);
    snprintf(mqtt_topic, 128, "%s/sensors/%s/alert_value", MQTT_ROOT, sensor_names[i]);
    snprintf(mqtt_value, 20, "%d", psensor->alert_value);
    mqtt_client.publish(mqtt_topic, mqtt_value);
    snprintf(mqtt_topic, 128, "%s/sensors/%s/alert_flag", MQTT_ROOT, sensor_names[i]);
    snprintf(mqtt_value, 20, "%d", psensor->alert_flag);
    mqtt_client.publish(mqtt_topic, mqtt_value);      
  }     
  Serial.println("Sensors have been published!");  
}

void publishDevices() {  
  device_state* pdevice_state;
  //
  for (int i = 0; i < DEVICES_COUNT; i++) {
    pdevice_state = getDeviceStateByType(i + 1);    
    if (!pdevice_state) continue;
    //
    snprintf(mqtt_topic, 128, "%s/devices/%s", MQTT_ROOT, device_names[i]);
    snprintf(mqtt_value, 20, "%d", pdevice_state->value);
    mqtt_client.publish(mqtt_topic, mqtt_value);
  }
  Serial.println("Devices have been published!");
}

void mqtt_publish_all() {  
  publishMode();
  publishSensors();
  publishDevices();
}

void mqtt_handle() {
  if (!(mqtt_client.connected() &&
        mqtt_client_sub.connected()))
    mqtt_init();
  //
  mqtt_client.loop();
  mqtt_client_sub.loop();
}
