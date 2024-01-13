#ifndef MQTT_MODULE_H
#define MQTT_MODULE_H

#define MQTT_CLIENT_ID "1863_evi"
#define MQTT_ROOT "gb_iot/1863_evi"

#define MQTT_RECONNECT_SEC 5
#define MQTT_RECONNECT_CNT 3

#define MQTT_MAX_SUBSCRIBE_TOPICS 10
#define MQTT_MAX_VALUE_LENGTH 255

struct mqtt_subscribe_topic {
  char path[MQTT_MAX_VALUE_LENGTH];
  char value[MQTT_MAX_VALUE_LENGTH];
}; 

extern mqtt_subscribe_topic mqtt_subscribe_topics[MQTT_MAX_SUBSCRIBE_TOPICS];

void mqtt_init();

void mqtt_callback(char* topic, byte *payload, unsigned int length);

void publishMode();

void publishSensors();

void publishDevices();

void publishPLC();

void mqtt_publish_all();

void mqtt_handle();

bool mqtt_is_connected();

void mqtt_subscribe_to_topic(char* new_path);

void mqtt_unsubscribe_from_topic(int index);

#endif