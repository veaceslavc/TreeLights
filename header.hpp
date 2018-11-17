#ifndef _HEADER_HPP_
#define _HEADER_HPP_

#define _TASK_STATUS_REQUEST    // Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
#include <TaskSchedulerDeclarations.h>

#define MQTT_ROOT_PATH    "/ha/tree"

#define MQTT_ROOT_TO_PATH MQTT_ROOT_PATH"/to"
#define MQTT_SCENE_TOPIC  MQTT_ROOT_TO_PATH"/scene"
#define MQTT_SPEED_TOPIC  MQTT_ROOT_TO_PATH"/speed"
#define MQTT_LISTEN_TOPIC MQTT_ROOT_TO_PATH"/#"

#define MQTT_ROOT_FROM_PATH   MQTT_ROOT_PATH"/from"
#define MQTT_SCENE_PUB_TOPIC  MQTT_ROOT_FROM_PATH"/scene"
#define MQTT_SPEED_PUB_TOPIC  MQTT_ROOT_FROM_PATH"/speed"


#define LIGHT_INIT_SCENE  7
#define CONNECT_TIMEOUT   30      // Seconds
#define CONNECT_OK        0       // Status of successful connection to WiFi
#define CONNECT_FAILED    (-99)   // Status of failed connection to WiFi

#define LEDPIN1   D0
#define LEDPIN2   D5
#define LEDPIN3   D6
#define LEDPIN4   D7

#define LEDPIN    D4      // Onboard LED pin - linked to WiFi

extern Scheduler ts;
extern Task tConnect;
//extern Task tMQTTClient;
extern Task tLED;
extern Task tLights;
extern Task tFFT;

extern void ledShowConnecting();
extern void ledShowError();
extern void ledShow(int delayOn, int deayOff);

extern void lightsSetup();
extern void lightsNextScene();
extern void lightsSetSpeed(int speedPercent);
extern void lightsSetScene(int newScene);
extern void displayLight(int mask);

extern void mqttSetup();
extern void mqttPublishScene(int sceneNum);
extern void mqttPublishSpeed(int speed);

extern void otaSetup();

extern void btnSetup();
#endif
