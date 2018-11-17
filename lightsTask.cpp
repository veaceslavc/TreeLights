#include <Arduino.h>
#include "header.hpp"

#define LIGHTS_INIT_SPEED 50

#define ALL_OFF_SCENE 0
#define ITERATE_SCENE 6
#define ALL_ON_SCENE  7
#define RANDOM_SCENE  8
#define MIC_SCENE     9

#define NUMBER_OF_SCENES 10
#define ITEMS_PER_SCENE 8

const byte scenes[][8] = {
  {0b1000, 0b0100, 0b0010, 0b0001, 0b1000, 0b0100, 0b0010, 0b0001},
  {0b1010, 0b0101, 0b1010, 0b0101, 0b1010, 0b0101, 0b1010, 0b0101},
  {0b0111, 0b1011, 0b1101, 0b1110, 0b0111, 0b1011, 0b1101, 0b1110},
  {0b1000, 0b1100, 0b1110, 0b1111, 0b0111, 0b0011, 0b0001, 0b0000},
  {0b0000, 0b1111, 0b0000, 0b1111, 0b0000, 0b1111, 0b0000, 0b1111}
};

int   sceneSet;
int   currentScene;
byte  sceneItemIdx = 0;

void lightsTaskLoop();
void sceneChanged();

Task tLights (TASK_SECOND - LIGHTS_INIT_SPEED *10, TASK_FOREVER, &lightsTaskLoop, &ts, true, NULL, NULL);

void lightsSetup() {
  pinMode (LEDPIN1, OUTPUT);
  pinMode (LEDPIN2, OUTPUT);
  pinMode (LEDPIN3, OUTPUT);
  pinMode (LEDPIN4, OUTPUT);
}

void lightsNextScene() {
  lightsSetScene( sceneSet+1 );
}

void lightsSetSpeed(int speedPercent) {
  tLights.setInterval(TASK_SECOND - speedPercent *10);
}

void lightsSetScene(int newScene) {
  newScene = newScene % NUMBER_OF_SCENES; // stay in the range of scene number
  Serial.print("Set lights scene: ");
  Serial.println(newScene);
  if (newScene != sceneSet) {
    sceneSet = newScene;
    sceneChanged();
//    tLights.disable(); // force call of onEnable to change the internal scene
    mqttPublishScene(newScene);
  }
  tLights.enableIfNot();
}

void sceneChanged() {
  if (sceneSet == ITERATE_SCENE) {
    currentScene = 1;
  } else {
    currentScene = sceneSet;
  }
  sceneItemIdx = 0;
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

    case RANDOM_SCENE:
      displayLight(random(0xF+1));
      break;
      
    case MIC_SCENE: // all OFF and stop
Serial.println("Mic Scene");
      displayLight(0b0000);
      tLights.disable();
      tFFT.enableIfNot();
      break;

    case ITERATE_SCENE: // iterate through scenes
      if ( (tLights.getRunCounter() % (ITEMS_PER_SCENE*10)) == 0 ) {
        currentScene = (currentScene % 4) + 1;
      }
    default:    
      byte lightMask = scenes[currentScene-1][sceneItemIdx];
      displayLight(lightMask);
      sceneItemIdx = ++sceneItemIdx % ITEMS_PER_SCENE;
  }

  if (sceneSet != MIC_SCENE) {
    tFFT.disable(); // disable FFT in case it is another scene
  }
}

void displayLight(int mask) {
//  Serial.print("lights: ");
  for (int i=3; i>=0; i--) {
    bool lightBit = (mask >> i) & 0b0001;
  
//    Serial.print(lightBit);
    switch (i) {
      case 0: digitalWrite(LEDPIN1, lightBit); break;
      case 1: digitalWrite(LEDPIN2, lightBit); break;
      case 2: digitalWrite(LEDPIN3, lightBit); break;
      case 3: digitalWrite(LEDPIN4, lightBit); break;
    }
  }
//  Serial.println();
}

