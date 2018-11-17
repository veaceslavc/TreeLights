#include "arduinoFFT.h" // Standard Arduino FFT library
#include "header.hpp"

/////////////////////////////////////////////////////////////////////////
#define SAMPLES 256              //Must be a power of 2
#define SAMPLING_FREQUENCY 100 //Hz, must be 10000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT.
#define amplitude 50
#define SAMPLING_PERIOD_US round(1000000 * (1.0 / SAMPLING_FREQUENCY))

/////////////////////////////////////////////////////////////////////////

double vReal[SAMPLES];
double vImag[SAMPLES];

double prevMaxVol;
byte lights;
byte faderCount;
byte faderRef;

arduinoFFT FFT = arduinoFFT();

void fftLoop();
void volumeLights();
void volumeLights2();

Task tFFT (SAMPLING_PERIOD_US / 1000, TASK_FOREVER, &fftLoop, &ts, false);

void fftSetup() {
  
}

void fftLoop() {
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = analogRead(A0); // A conversion takes about 1mS on an ESP8266
    vImag[i] = 0;
  }
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  
  volumeLights2();
}

void volumeLights(){
  double maxVol = 100; 
  for (int i = 2; i < (SAMPLES/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
    if (vReal[i] > 100) { // Add a crude noise filter, 4 x amplitude or more
      if (vReal[i] > maxVol) maxVol = vReal[i];
    }
  }
  
  if (abs(maxVol - prevMaxVol) > 10) {
    lights = ((byte)maxVol & 0xF0) >> 4;
    prevMaxVol = maxVol;
    Serial.print("maxVol=");
    Serial.print(maxVol);
  }
}

void volumeLights2(){
  double maxVol = 0; 
  for (int i = 2; i < (SAMPLES/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
    if (vReal[i] > 300) { // Add a crude noise filter, 4 x amplitude or more
      maxVol += vReal[i];
    }
  }
  maxVol = maxVol /  SAMPLES/2;
  if (abs(maxVol - prevMaxVol) > 1) {
    lights = ((byte)maxVol & 0xF0) >> 4;
    //lights = (((byte)maxVol) & 0x0F);
    lights = map(maxVol, 5, 120, 0, 0xF);
    prevMaxVol = maxVol;
    Serial.print("maxVol=");
    Serial.print(maxVol);
    Serial.print(" mappedVol=");
    Serial.print(map(maxVol, 1000, 30000, 0, 0xF));
    Serial.print(" lights=");
    Serial.print(lights);
    Serial.println();
    if (lights != 0) {
      displayLight(lights);
      faderCount = 0; faderRef = lights;
    } else {
      if ( (++faderCount % 3) == 0 ) {
        faderRef = faderRef >> 1;
        displayLight(faderRef);
      }
    }
  }
}
