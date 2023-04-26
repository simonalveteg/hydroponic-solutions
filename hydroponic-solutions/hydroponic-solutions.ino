#include <LiquidCrystal.h>
#include <Streaming.h>
#include <OneButton.h>
#include "Sonar.h"
#include "Conductivity.h"
#include "Pump.h"


#define RUN_INTERVAL 5000
#define RUN_TIME 30000
#define NUTRIENT_RUN_TIME 30000

#define STATE_MAIN 0
#define STATE_STIR 1
#define STATE_NUTRIENT 2
#define STATE_WAIT 3

int state;

const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Conductivity conductivity(A5, A1, A2, A0);
Sonar sonar(A3, A4, 200);
Pump waterPump(4);
Pump nutrientPump(5);
Pump stirPump(6);

int waterLevel = 0;  // keep track of water level (in terms of percentage filled?)

float ECsetPoint = 1.6;  // Preferred value of EC is 1.6
int screenState = 0;

unsigned long waitTimer = 0, pumpTimer = 0;

const int screenPin = 1, ecUpPin = 2, ecDownPin = 3;

OneButton btnScreen = OneButton(screenPin, true, true);
OneButton btnUp = OneButton(ecUpPin, true, true);
OneButton btnDown = OneButton(ecDownPin, true, true);

void setup() {
  Serial.begin(9600);
  buttonSetup();
  conductivity.setup();
  lcd.begin(16, 2);
  // updateLCD();
  for (char i = 47; i < 127; i++)  // send 80 consecutive displayable characters to the LCD
  {
    lcd.print(i);
    delay(100);  // this delay allows you to observe the addressing sequence
  }
  state = STATE_MAIN;
}

void loop() {
  btnScreen.tick();
  btnUp.tick();
  btnDown.tick();
  updateLCD();
  switch (state) {
    case STATE_MAIN:
      Serial << "=== MAIN STATE ===" << endl;
      if (checkWaterLevel()) {
        waterPump.start();
        stirPump.start();
        pumpTimer = millis();
        state = STATE_STIR;
      } else {
        waitTimer = millis();
        state = STATE_WAIT;
      }
      break;
    case STATE_WAIT:
      Serial << "=== WAIT STATE ===" << endl;
      if (millis() - waitTimer > RUN_INTERVAL) {
        state = STATE_MAIN;
      }
      break;
    case STATE_STIR:
      Serial << "=== STIR STATE ===" << endl;

      if (millis() - pumpTimer > RUN_TIME) {
        waterPump.stop();
        stirPump.stop();

        if (checkEC()) {
          waitTimer = millis();
          state = STATE_WAIT;
        } else {
          nutrientPump.start();
          pumpTimer = millis();
          state = STATE_NUTRIENT;
        }
      }
      break;
    case STATE_NUTRIENT:
      Serial << "=== NUTRIENT STATE ===" << endl;
      Serial << "Nutrient solution low, starting nutrient pump." << endl;
      if (millis() - pumpTimer > NUTRIENT_RUN_TIME) {
        nutrientPump.stop();
        state = STATE_MAIN;
      }
      break;
    default:
      Serial << "Uh oh.." << endl;
  }
}

void updateLCD() {
  Serial << "Update Display" << endl;
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial << "ECsetPoint: " << ECsetPoint << " ScreenState: " << screenState << endl;
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
  waterLevel = (waterLevel < 0) ? 0 : waterLevel;
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

void buttonSetup() {
  btnScreen.attachClick([]() {
    screenState += 1;
  });
  btnUp.attachClick([]() {
    Serial.println("PRESS UP");
    ECsetPoint += 0.1;
  });
  btnDown.attachClick([]() {
    ECsetPoint -= 0.1;
  });
}
