#include <Arduino.h>
#include "header.hpp"

#define BTN_PIN D3
#define LONG_PRESS_INTERVAL 1000

void btnLoop();

Task  tButton     (TASK_MILLISECOND * 50, TASK_FOREVER, &btnLoop, &ts, true);

boolean buttonActive = false;
boolean longPressActive = false;

long buttonTimer = 0;

void btnSetup() {
Serial.println("Button Setup");
  pinMode(BTN_PIN, INPUT_PULLUP);
}

void btnLoop() {
  if (digitalRead(BTN_PIN) == LOW) {

    if (buttonActive == false) {

      buttonActive = true;
      buttonTimer = millis();

    }

    if (millis() - buttonTimer > LONG_PRESS_INTERVAL) {

      longPressActive = true;
      buttonTimer = millis();
      Serial.println("Button long press");
    }

  } else {

    if (buttonActive == true) {

      if (longPressActive == true) {

        longPressActive = false;

      } else {

        Serial.println("Button short press");
        lightsNextScene();

      }

      buttonActive = false;

    }

  }
}

