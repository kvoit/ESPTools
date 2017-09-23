//#define DEBUGLib_DEBUG
#include <DEBUGLib.h> //https://github.com/kvoit/DEBUGLib
#include <INTERVAL.h> //https://forum.arduino.cc/index.php?topic=413734.0
#include <ESPTools.h> //https://github.com/knolleary/pubsubclient
#include <ArduinoOTA.h>

bool mqtt_reconnect(PubSubClient &mqttclient, const char *device_name, const char *user_name, const char *mqtt_pw, const char *mqtt_topic_tree) {
 // Loop until we're reconnected
    
  bool returnval = true;
  if (!mqttclient.connected()) {
    DEBUG_PRINT("Attempting MQTT connection...");
    // Attempt to connect
    returnval=mqttclient.connect(device_name, user_name, mqtt_pw);
    if (returnval) {
      DEBUG_PRINTLN("connected");
      mqttclient.subscribe(mqtt_topic_tree);
      DEBUG_PRINT("Subscribed to: ");
      DEBUG_PRINTLN(mqtt_topic_tree);
    } else {
      DEBUG_PRINT("failed, rc=");
      DEBUG_PRINT(mqttclient.state());
      DEBUG_PRINTLN(" try again in 5 seconds");
    }
  }
  return returnval;
}

bool mqtt_check(PubSubClient &mqttclient, uint16_t mqttReconnectInterval, const char *device_name, const char *user_name, const char *mqtt_pw, const char *mqtt_topic_tree) {
    bool returnval = true;
    if (!mqttclient.connected()) {
        INTERVAL(mqttReconnectInterval)
        {
            returnval = mqtt_reconnect(mqttclient, device_name, user_name, mqtt_pw, mqtt_topic_tree);
        }
    } else {
        mqttclient.loop();
    }
    return returnval;
}

void startWifi(const char* ssid, const char* password, const char* device_name)
{
  digitalWrite ( LED_BUILTIN, LOW );
  WiFi.mode(WIFI_STA);
  WiFi.softAPdisconnect(true);
  WiFi.begin ( ssid, password );
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(device_name);
  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    digitalWrite ( LED_BUILTIN, !digitalRead(LED_BUILTIN) );
    DEBUG_PRINT( "." );
  }

  DEBUG_PRINTLN ( "" );
  DEBUG_PRINT ( "Connected to " );
  DEBUG_PRINTLN ( ssid );
  DEBUG_PRINT ( "IP address: " );
  DEBUG_PRINTLN ( WiFi.localIP() );

  if ( MDNS.begin ( device_name ) ) {
    DEBUG_PRINTLN ( "MDNS responder started" );
  }

  digitalWrite ( LED_BUILTIN, HIGH );
}

void configArduinoOTA(const char *device_name, const char* ota_password)
{
    // Port defaults to 8266
    ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    ArduinoOTA.setHostname(device_name);

    // No authentication by default
    ArduinoOTA.setPassword(ota_password);

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
    else // U_SPIFFS
        type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    DEBUG_PRINTLN("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
    DEBUG_PRINTLN("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_PRINTF("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
    DEBUG_PRINTF("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) DEBUG_PRINTLN("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) DEBUG_PRINTLN("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) DEBUG_PRINTLN("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) DEBUG_PRINTLN("Receive Failed");
    else if (error == OTA_END_ERROR) DEBUG_PRINTLN("End Failed");
    });
    ArduinoOTA.begin();
}
