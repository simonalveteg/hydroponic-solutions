#ifndef NUTRIENT_H
#define NUTRIENT_H

#include "Arduino.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include <Streaming.h>

class Nutrient {
private:
  OneWire *oneWire;
  DallasTemperature *sensors;

  int R1, Ra, DATA_PIN, GROUND_PIN, POWER_PIN, STEP_PIN;
  bool polarity;
  float temperatureCoef, cellConstant, Vin, Rc, k, m;
  void pump(float ml);
public:
  float ec, ec25, temperature, concentration;
  Nutrient(int oneWireBusPin, int ECPin, int ECGround, int ECPower, int stepPin);
  void setup();
  void print();
  void read();
  float refill(float volume, float targetConcentration);
};

#endif