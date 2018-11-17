#include <Arduino.h>
#include "header.hpp"

void ledShowConnecting();
void ledShowError();
void ledCallback();
bool ledOnEnable();
void ledOnDisable();
void ledOn();
void ledOff();

Task  tLED        (TASK_IMMEDIATE, TASK_FOREVER, &ledCallback, &ts, false, &ledOnEnable, &ledOnDisable);

long  ledDelayOn, ledDelayOff;

void ledShow(int delayOn, int deayOff) {
  ledDelayOn = delayOn;
  ledDelayOff = deayOff;
  tLED.enableIfNot();
}

void ledShowConnecting() {
  ledDelayOn = TASK_SECOND / 4;
  ledDelayOff = TASK_SECOND / 4;
  tLED.enableIfNot();
}

void ledShowError() {
  ledDelayOn = TASK_SECOND / 32;   // Blink LEDs quickly due to error
  ledDelayOff = TASK_SECOND / 8;
  tLED.enableIfNot();
}
/**
 * Flip the LED state based on the current state
 */
bool ledState;
void ledCallback() {
  if ( ledState ) ledOff();
  else ledOn();
}

/**
 * Make sure the LED starts lit
 */
bool ledOnEnable() {
  ledOn();
  return true;
}

/**
 * Make sure LED ends dimmed
 */
void ledOnDisable() {
  ledOff();
}

/**
 * Turn LEDs on. 
 * Set appropriate delay.
 * PLEASE NOTE: NodeMCU onbaord LED is active-low
 */
void ledOn() {
  ledState = true;
  digitalWrite(LEDPIN, LOW);
  tLED.delay( ledDelayOn );
}

/**
 * Turn LEDs off. 
 * Set appropriate delay.
 * PLEASE NOTE: NodeMCU onbaord LED is active-low
 */
void ledOff() {
  ledState = false;
  digitalWrite(LEDPIN, HIGH);
  tLED.delay( ledDelayOff );
}


