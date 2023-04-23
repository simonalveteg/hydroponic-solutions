#ifndef PUMP_H
#define PUMP_H

#include "Arduino.h"

class Pump {
private:
  int pin;
public:
  Pump(int pin);
  void start(int millis);
};

#endif