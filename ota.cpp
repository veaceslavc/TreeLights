#include "header.hpp"
#include <ArduinoOTA.h>

#define OTA_LOOP_INTERVAL TASK_MILLISECOND * 10

Scheduler ts;

void otaEnable();
void otaLoop();

Task  tOTA    (&otaEnable, &ts);

void otaSetup() {
Serial.println("OTA Setup");
    tOTA.waitFor( tConnect.getInternalStatusRequest() ); // Wait for WiFi to connect
}

void otaEnable() {
Serial.println("OTA OnEnable");
    // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("esp8266-TreeLights");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int percent = (progress / (total / 100));
    if ( (percent % 5) == 0 ) {
      Serial.printf("Progress: %u%%\r\n", percent);
    }
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
  Serial.print("OTA IP address: ");
  Serial.println(WiFi.localIP());

  tOTA.set( OTA_LOOP_INTERVAL, TASK_FOREVER, &otaLoop );  // enable forever loop on ArduinoOTA library
}

void otaLoop() {
//Serial.println("OTA loop");
  ArduinoOTA.handle();
}

