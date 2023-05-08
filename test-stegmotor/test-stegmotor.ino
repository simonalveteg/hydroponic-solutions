#include <Streaming.h>
#include "Conductivity.h"
#include "Nutrient.h"

#define STEP_PIN 13
#define TIME 500

Nutrient nutrient(A5, A1, A2, A0, STEP_PIN);
float volume = 0, ec = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(STEP_PIN, OUTPUT);
  nutrient.setup();
  delay(10000);
  //pump(1.0);
}

void loop() {
  measureLoop();
  nutrient.read();
  //Serial << nutrient.ec25 << ", " << nutrient.concentration << ", " << nutrient.temperature << endl;
  nutrient.refill(1, 2.0);
  delay(10000);
}

void measureLoop() {
  for (int i = 0; i < 100; i++) {
    nutrient.read();
    Serial << nutrient.ec25 << "mS/cm, " << nutrient.concentration << "ml/l, " << nutrient.temperature <<"deg C" << endl;
  }
}

void pump(float ml) {
  int steps = ml / (0.10857 / 200);
  Serial << "STEPS TO TAKE: " << steps << endl;
  for (int i = 0; i < steps; i++) {  
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(500);
  }
}