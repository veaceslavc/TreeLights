#include "header.hpp"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

Scheduler ts;

bool mqttInit();
void mqttConnect();
void mqttLoop();

// Tasks running on events
Task  tMQTTClient  (&mqttConnect, &ts, &mqttInit);

const char* mqtt_server = "192.168.1.13";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


/*
 * MQTT client
 */

 void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String strTopic = String(topic);
  String message = (char*)payload;

  if (strTopic.equals(MQTT_SCENE_TOPIC)) {
    
    Serial.println("Received scene num: " + message);
    int sceneNum = message.toInt();
    lightsSetScene(sceneNum);
    
  } else if (strTopic.equals(MQTT_SPEED_TOPIC)) {

    Serial.println("Received speed: " + message);
    int speedNum = message.toInt();
    lightsSetSpeed(speedNum);
  }
}

bool mqttInit() {
  Serial.println(F("Initializing MQTT client"));

  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(mqttCallback);

  tMQTTClient.setInterval(TASK_SECOND * 5);
  tMQTTClient.setIterations(CONNECT_TIMEOUT / 5);
  return true;
}

void mqttConnect() {
  if ( tConnect.getInternalStatusRequest()->getStatus() != CONNECT_OK ) {  // Check status of the Connect Task
    Serial.println(F("Cannot connect to MQTT - WiFi not connected."));
    tMQTTClient.disable();
    return;
  }

  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (mqttClient.connect("TreeLights")) {
    Serial.println("connected");
    tLED.disable();

    mqttClient.subscribe(MQTT_LISTEN_TOPIC);
    tMQTTClient.set( TASK_MILLISECOND * 100, TASK_FOREVER, &mqttLoop );  // enable forever loop on MQTT library
  } else {
    Serial.print("failed, rc=");
    Serial.println(mqttClient.state());
    
    if ( tMQTTClient.isLastIteration() ) {
     Serial.println(F("Giving up connecting to MQTT!"));
     ledShowError();
   } 
  }
}

void mqttLoop() {
  mqttClient.loop();
}
