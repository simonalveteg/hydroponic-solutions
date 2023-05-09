#include "Nutrient.h"

Nutrient::Nutrient(int oneWireBusPin, int ECDataPin, int ECGroundPin, int ECPowerPin, int stepPin)
  : R1(1000), Ra(25), temperatureCoef(0.019), cellConstant(1.67), Vin(5.0), polarity(true),
    DATA_PIN(ECDataPin), GROUND_PIN(ECGroundPin), POWER_PIN(ECPowerPin), STEP_PIN(stepPin), k(0.4045), m(0.1962) {
  oneWire = new OneWire(oneWireBusPin);
  sensors = new DallasTemperature(oneWire);
}

void Nutrient::pump(float ml) {
  int steps = ml / (0.10857 / 200);
  Serial << "STEPS TO TAKE: " << steps << endl;
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(500);
  }
}

void Nutrient::setup() {
  pinMode(DATA_PIN, INPUT);
  pinMode(POWER_PIN, OUTPUT);     // Setting pin for sourcing current
  pinMode(GROUND_PIN, OUTPUT);    // Setting pin for sinking current
  digitalWrite(GROUND_PIN, LOW);  // We can leave the ground connected permanantly -- not connected to regular ground pin to reduce noise?

  delay(100);
  sensors->begin();
  delay(100);

  R1 += Ra;
}

void Nutrient::print() {
  Serial << " Rc: " << Rc << " ec25: " << ec25 << ", " << temperature << " *C, " << concentration << "ml/l" << endl;
}

void Nutrient::read() {
  sensors->requestTemperatures();
  temperature = sensors->getTempCByIndex(0);
  float Vdrop;

  if (polarity) {
    digitalWrite(POWER_PIN, HIGH);
    float raw = analogRead(DATA_PIN);
    raw = analogRead(DATA_PIN);
    digitalWrite(POWER_PIN, LOW);

    Vdrop = (Vin * raw) / 1024.0;
  } else {
    digitalWrite(GROUND_PIN, HIGH);
    float raw = analogRead(DATA_PIN);
    raw = analogRead(DATA_PIN);
    digitalWrite(GROUND_PIN, LOW);

    Vdrop = Vin - (Vin * raw) / 1024.0;
  }

  Rc = (Vdrop * R1) / (Vin - Vdrop) - Ra;
  ec = 1000 / (Rc * cellConstant);

  ec25 = ec / (1 + temperatureCoef * (temperature - 25.0));

  concentration = (ec25 - m) / k;
  polarity = !polarity;
}

void Nutrient::refill(float volume, float targetConcentration) {
  float mlAdd = (targetConcentration - concentration) * volume;
  Serial << mlAdd << " ml will be added!" << endl;
  pump(mlAdd);
}