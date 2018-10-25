#include <Arduino.h>
#include "header.hpp"

#define LIGHTS_INIT_SPEED 50

#define ALL_OFF_SCENE 0
#define ALL_ON_SCENE  7
#define ITERATE_SCENE 6

const byte scenes[][8] = {
  {0b1000, 0b0100, 0b0010, 0b0001, 0b1000, 0b0100, 0b0010, 0b0001},
  {0b1010, 0b0101, 0b1010, 0b0101, 0b1010, 0b0101, 0b1010, 0b0101},
  {0b0111, 0b1011, 0b1101, 0b1110, 0b0111, 0b1011, 0b1101, 0b1110},
  {0b1000, 0b1100, 0b1110, 0b1111, 0b0111, 0b0011, 0b0001, 0b0000},
  {0b0000, 0b1111, 0b0000, 0b1111, 0b0000, 0b1111, 0b0000, 0b1111}
};

int  sceneSet;
int  currentScene;
byte  sceneItemIdx = 0;

void lightsTaskLoop();
bool lightsTaskEnable();
void displayLight(int mask);

Task tLights (TASK_SECOND - LIGHTS_INIT_SPEED *10, TASK_FOREVER, &lightsTaskLoop, &ts, true, &lightsTaskEnable, NULL);

void lightsSetSpeed(int speedPercent) {
  tLights.setInterval(TASK_SECOND - speedPercent *10);
}

void lightsSetScene(int newScene) {
  if (newScene != sceneSet) {
    tLights.disable();
    sceneSet = newScene;
    tLights.enable();
  }
}

bool lightsTaskEnable() {
  pinMode (LEDPIN1, OUTPUT);
  pinMode (LEDPIN2, OUTPUT);
  pinMode (LEDPIN3, OUTPUT);
  pinMode (LEDPIN4, OUTPUT);

  if (sceneSet == ITERATE_SCENE) {
    currentScene = 1;
  } else {
    currentScene = sceneSet;
  }
  sceneItemIdx = 0;
  return true;
}

void lightsTaskLoop() {
  switch (sceneSet) {
    case ALL_OFF_SCENE: // all OFF and stop
      displayLight(0b0000);
      tLights.disable();
      break;

    case ALL_ON_SCENE: // all OFF and stop
      displayLight(0b1111);
      tLights.disable();
      break;
      
    case ITERATE_SCENE: // iterate through scenes
      if ( (tLights.getRunCounter() % (8*10)) == 0 ) {
        currentScene = (currentScene % 4) + 1;
      }
    default:    
      byte lightMask = scenes[currentScene-1][sceneItemIdx];
      displayLight(lightMask);
      sceneItemIdx = ++sceneItemIdx % 8;
  }
}

void displayLight(int mask) {
  Serial.print("lights: ");
  for (int i=3; i>=0; i--) {
    bool lightBit = (mask >> i) & 0b0001;
  
    Serial.print(lightBit);
    switch (i) {
      case 0: digitalWrite(LEDPIN1, !lightBit);
      case 1: digitalWrite(LEDPIN2, lightBit);
      case 2: digitalWrite(LEDPIN3, lightBit);
      case 3: digitalWrite(LEDPIN4, lightBit);
    }
  }
  Serial.println();
}

