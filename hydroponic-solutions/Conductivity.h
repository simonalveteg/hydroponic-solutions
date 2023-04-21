#ifndef CONDUCTIVITY_H
#define CONDUCTIVITY_H

#include "Arduino.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include <Streaming.h> // for serial << usage

class Conductivity {
private:
  OneWire *oneWire;
  DallasTemperature *sensors;

  int R1, Ra, dataPin, groundPin, powerPin;
  float PPMconversion, TemperatureCoef, K, Vin;

public:
  float Rc, EC, EC25, Temperature;
  int ppm;
  Conductivity(int oneWireBusPin, int ECPin, int ECGround, int ECPower);
  void setup();
  void print();
  void read();
};

#endif