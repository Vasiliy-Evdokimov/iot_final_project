#ifndef WEB_MODULE_H
#define WEB_MODULE_H

#include <WebServer.h>

extern WebServer server;

void handleRoot();

void handleGetStatus();

void handleSetMode();

void handleSetAlerts();

void handleSetDevices();

void handleSetPlcMasks();

void handleDelMqttPath();

void handleAddMqttPath();

void handleSetPublishFlags();

void wifi_init();

void web_init();

void web_handle();

#endif