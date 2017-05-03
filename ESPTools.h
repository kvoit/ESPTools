#ifndef ESPTools_h
#define ESPTools_h
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient

bool mqtt_reconnect(PubSubClient &mqttclient, const char *device_name, const char *user_name, const char *mqtt_pw, const char *mqtt_topic_tree);
void startWifi(const char* ssid, const char* password, const char* device_name);
bool mqtt_check(PubSubClient &mqttclient, uint16_t mqttReconnectInterval, const char *device_name, const char *user_name, const char *mqtt_pw, const char *mqtt_topic_tree);
void configArduinoOTA(const char *device_name, const char* ota_password);

#endif
