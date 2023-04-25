#include <LiquidCrystal.h>
#include <Streaming.h>
#include "Moisture.h"
#include "Sonar.h"
#include "Conductivity.h"
#include "Pump.h"


#define RUN_INTERVAL 5000
#define RUN_TIME 30000

#define STATE_MAIN 0
#define STATE_STIR 1
#define STATE_NUTRIENT 2
#define STATE_WAIT 3

int state;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Sonar sonar(1, 0, 200);
Conductivity conductivity(8, A1, A2, A0);
Pump waterPump(6);
Pump nutrientPump(7);
Pump stirPump(9);

int waterLevel = 0;  // keep track of water level (in terms of percentage filled?)

float ECsetPoint = 1.6;  // Preferred value of EC is 1.6

void setup() {
  Serial.begin(9600);
  conductivity.setup();
  lcd.begin(16, 2);
  updateLCD();
  state = STATE_MAIN;
}

void loop() {
  switch (state) {
    case STATE_MAIN:
      Serial << "=== MAIN STATE ===" << endl;
      if (checkWaterLevel()) {
        state = STATE_STIR;
      } else {
        state = STATE_WAIT;
      }
      updateLCD();
      break;
    case STATE_STIR:
      Serial << "=== STIR STATE ===" << endl;
      waterPump.start(RUN_TIME);
      stirPump.start(RUN_TIME);
      if (checkEC()) {
        state = STATE_WAIT;
      } else {
        state = STATE_NUTRIENT;
      }
      updateLCD();
      break;
    case STATE_NUTRIENT:
      Serial << "=== NUTRIENT STATE ===" << endl;
      Serial << "Nutrient solution low, starting nutrient pump." << endl;
      nutrientPump.start(2000);
      state = STATE_MAIN;
      break;
    case STATE_WAIT:
      Serial << "=== WAIT STATE ===" << endl;
      delay(RUN_INTERVAL);
      state = STATE_MAIN;
      break;
    default:
      Serial << "Uh oh.." << endl;
  }
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd << "EC: " << conductivity.EC25;
  lcd.setCursor(0, 1);
  lcd << "Water Level " << waterLevel << "%";
}

/**
* Check the water level. Return true if it's above the minimum height and false otherwise.
*/
bool checkWaterLevel() {
  unsigned int sonarDistance = sonar.read();
  float maxDistance = 8;
  float minDistance = 1;
  waterLevel = 100 - 100 * (sonarDistance - minDistance) / (maxDistance - minDistance);
  Serial << "Distance to water: " << sonarDistance << "cm. Water level: " << waterLevel << "%" << endl;
  return sonarDistance >= minDistance && sonarDistance <= maxDistance;
}

/**
* Check EC level. Return true if value is above setpoint.
*/
bool checkEC() {
  conductivity.read();
  conductivity.print();
  return conductivity.EC25 > ECsetPoint;
}
