#include "Arduino.h"

#include "utils.hpp"
#include "mqtt_module.hpp"
#include "auth.hpp"
#include "certs.hpp"
#include "uart_module.hpp"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure wifi_client;
WiFiClientSecure wifi_client_sub;
PubSubClient mqtt_client;
PubSubClient mqtt_client_sub;

char mqtt_topic[128];
char mqtt_value[20];

bool use_mqtt = false;

bool mqtt_is_connected()
{
  return mqtt_client.connected() && mqtt_client_sub.connected();
}

void mqtt_init() 
{  
  if (!use_mqtt) return;
  //
  int reconnect = MQTT_RECONNECT_CNT;
  while (!mqtt_is_connected() || !reconnect--)
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
    bool connected = true;    
    //
    con_print("MQTT connecting... ");    
    if (mqtt_client.connect(clientId.c_str())) {
      con_println("MQTT connected!");
    } else {
      con_print("Failed, status code = ");
      con_println(String(mqtt_client.state()));
      connected = false;
    }
    //
    con_print("MQTT_sub connecting... ");
    if (mqtt_client_sub.connect(clientId.c_str())) {
      con_println("MQTT_sub connected!");
      mqtt_client_sub.subscribe(MQTT_SUBSCRIBE_PATH);
    } else {
      con_print("Failed, status code = ");
      con_println(String(mqtt_client_sub.state()));
      connected = false;
    }
    //
    if (!connected && reconnect) {
      con_println(" Try again in " + String(MQTT_RECONNECT_SEC) + " seconds...");
      delay(MQTT_RECONNECT_SEC * 1000);
    }  
  }
}

void mqtt_callback(char* topic, byte *payload, unsigned int length) 
{
  if (!use_mqtt) return;
  //
  con_println("-------new message from broker-----");
  con_print("channel:");
  con_println(topic);
  con_print("data:");
  //  
  Serial.write(payload, length);
  //
  con_println("");
}

void publishMode() 
{
  if (!use_mqtt) return;
  //
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
  con_println("Mode has been published!");
}

void publishSensors() 
{
  if (!use_mqtt) return;
  //
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
  con_println("Sensors have been published!");  
}

void publishDevices()
{
  if (!use_mqtt) return;
  //
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
  con_println("Devices have been published!");
}

void mqtt_publish_all() 
{
  if (!use_mqtt) return;
  //  
  publishMode();
  publishSensors();
  publishDevices();
}

void mqtt_handle() 
{
  if (!use_mqtt) return;
  //
  if (!mqtt_is_connected()) 
  {
    con_println("MQTT not connected!");
    mqtt_init();
  }    
  //
  mqtt_client.loop();
  mqtt_client_sub.loop();
}
