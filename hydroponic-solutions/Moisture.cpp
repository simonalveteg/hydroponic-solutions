#include "Moisture.h"

Moisture::Moisture(int pin, int vccPin)
  : _pin(pin), _vccPin(vccPin) {}

void Moisture::setup() {
  pinMode(_vccPin, OUTPUT);
  digitalWrite(_vccPin, LOW);
}
int Moisture::read() {
  digitalWrite(_vccPin, HIGH);   // power the sensor
  delay(100);                    // make sure the sensor is powered
  int value = analogRead(_pin);  // read the value from the sensor:
  digitalWrite(_vccPin, LOW);    // stop power
  return value;
}