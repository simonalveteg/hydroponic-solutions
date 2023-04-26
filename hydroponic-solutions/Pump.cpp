#include "Pump.h"

Pump::Pump(int pin) : pin(pin) {
  pinMode(pin, OUTPUT);
}

void Pump::start() {
  digitalWrite(pin, HIGH);
}

void Pump::stop() {
  digitalWrite(pin, LOW);
}