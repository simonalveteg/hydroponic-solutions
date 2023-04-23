#include "Pump.h"

Pump::Pump(int pin) : pin(pin) {
  pinMode(pin, OUTPUT);
}

void Pump::start(int millis) {
  digitalWrite(pin, HIGH);
  delay(millis);
  digitalWrite(pin, LOW);
}