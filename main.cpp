#include <Arduino.h>
#include "header.hpp"

#include <ESP8266WiFi.h>


// Callback methods prototypes
void connectInit();
void doConnect();
void connectCheck();

// Tasks

Task  tConnect    (TASK_SECOND, TASK_FOREVER, &connectInit, &ts, true);


// Replace with WiFi parameters of your Access Point/Router:
const char *ssid  =  "SAS";
const char *pwd   =  "xxxxx";


void setup() {
  Serial.begin(74880);
  Serial.println(F("Christmas Tree Lights"));
  Serial.println(F("=========================================================="));
  Serial.println();

  pinMode (LEDPIN, OUTPUT);
  pinMode (LEDPIN1, OUTPUT);
  pinMode (LEDPIN2, OUTPUT);
  pinMode (LEDPIN3, OUTPUT);
  pinMode (LEDPIN4, OUTPUT);

  otaSetup();
  mqttSetup();
  btnSetup();
  lightsSetup();
  
  lightsSetScene(LIGHT_INIT_SCENE);
}

void loop() {
  ts.execute();                   // Only Scheduler should be executed in the loop
}

/**
   Initiate connection to the WiFi network
*/
void connectInit() {
  Serial.println(F("WiFi connectInit."));

  WiFi.mode(WIFI_STA);
  WiFi.hostname("TreeLights");
  WiFi.begin(ssid, pwd);
  yield();

  ledShowConnecting();
  tConnect.yield(&doConnect);            // This will pass control back to Scheduler and then continue with connection checking
}

/**
   Periodically check if connected to WiFi
   Re-request connection every 5 seconds
   Stop trying after a timeout
*/
void doConnect() {
  Serial.println(F("WiFi connectCheck."));

  if (WiFi.status() == WL_CONNECTED) {                // Connection established
    Serial.print(F("WiFi connected to AP. Local ip: "));
    Serial.println(WiFi.localIP());
    tLED.disable();
    tConnect.disable();
    //tConnect.yield(&connectCheck); to test and enable
  }
  else {

    if (tConnect.getRunCounter() % 5 == 0) {          // re-request connection every 5 seconds

      Serial.print(millis());
      Serial.println(F(": Re-requesting connection to AP..."));

      WiFi.disconnect(true);
      yield();                                        // This is an esp8266 standard yield to allow linux wifi stack run
      WiFi.hostname("esp8266");
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, pwd);
      yield();                                        // This is an esp8266 standard yield to allow linux wifi stack run
    }

    if (tConnect.getRunCounter() == CONNECT_TIMEOUT) {  // Connection Timeout
      tConnect.getInternalStatusRequest()->signal(CONNECT_FAILED);  // Signal unsuccessful completion
      tConnect.disable();

      Serial.println(F("Unable to connect to WiFi."));
      ledShowError();
    }
  }
}

void connectCheck() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("WiFi disconnect detected."));
    ledShowConnecting();
    tConnect.yield(&doConnect);
    
  }
}

