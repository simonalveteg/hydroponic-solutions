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
}

void loop() {
  // measureLoop();
  nutrient.read();
  Serial << nutrient.ec25 << ", " << nutrient.concentration << ", " << nutrient.temperature << endl;
}

void measureLoop() {
  float totalEC = 0;
  for (int i = 0; i < 50; i++) {
    nutrient.read();
    totalEC += nutrient.ec25;
  }
  ec = totalEC / 50;

  Serial << volume << "," << ec << endl;

  pump(1);
  volume += 0.10857;
  delay(60000);
}

void pump(float varv) {
  for (int i = 0; i < 200 * varv; i++) {  // gjorde en loop av nån anledning
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(TIME);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(TIME);
  }
}