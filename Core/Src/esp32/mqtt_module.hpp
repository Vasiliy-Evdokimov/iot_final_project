#ifndef MQTT_MODULE_H
#define MQTT_MODULE_H

#define MQTT_CLIENT_ID "1863_evi"
#define MQTT_ROOT "gb_iot/1863_evi"

#define MQTT_SUBSCRIBE_PATH "gb_iot/1863_evi/sub_test"

void mqtt_init();

void mqtt_callback(char* topic, byte *payload, unsigned int length);

void publishMode();

void publishSensors();

void publishDevices();

void mqtt_publish_all();

void mqtt_handle();

#endif