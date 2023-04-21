#ifndef MOISTURE_H
#define MOISTURE_H
#include <Arduino.h>

class Moisture {
private:
  int _pin, _vccPin;
public:
  Moisture(int pin, int vccPin);
  void setup();
  int read();
};

#endif