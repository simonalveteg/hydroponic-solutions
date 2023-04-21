#include "Conductivity.h"

Conductivity::Conductivity(int oneWireBusPin, int ECDataPin, int ECGroundPin, int ECPowerPin)
  : R1(1000), Ra(25), PPMconversion(0.7), TemperatureCoef(0.019), K(1.67),
    dataPin(ECDataPin), groundPin(ECGroundPin), powerPin(ECPowerPin) {
  oneWire = new OneWire(oneWireBusPin);
  sensors = new DallasTemperature(oneWire);
}

void Conductivity::setup() {
  pinMode(dataPin, INPUT);
  pinMode(powerPin, OUTPUT);
  pinMode(groundPin, OUTPUT);
  digitalWrite(groundPin, LOW);

  delay(100);
  sensors->begin();
  delay(100);

  R1 += Ra;
}

void Conductivity::print() {
  Serial.print("Rc: ");
  Serial.print(Rc);
  Serial.print(" EC: ");
  Serial.print(EC25);
  Serial.print(" Siemens  ");
  Serial.print(ppm);
  Serial.print(" ppm  ");
  Serial.print(Temperature);
  Serial.println(" *C ");
}

void Conductivity::read() {
  sensors->requestTemperatures();
  Temperature = sensors->getTempCByIndex(0);

  digitalWrite(powerPin, HIGH);
  float raw = analogRead(dataPin);
  raw = analogRead(dataPin);
  digitalWrite(powerPin, LOW);

  float Vdrop = (Vin * raw) / 1024.0;
  Rc = (Vdrop * R1) / (Vin - Vdrop);
  Rc = Rc - Ra;
  EC = 1000 / (Rc * K);

  EC25 = EC / (1 + TemperatureCoef * (Temperature - 25.0));
  ppm = (EC25) * (PPMconversion * 1000);
}