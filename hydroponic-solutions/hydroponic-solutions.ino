#include <LiquidCrystal.h>
#include <Streaming.h>
#include <OneButton.h>
#include <EEPROM.h>
#include "Sonar.h"
#include "Conductivity.h"
#include "Pump.h"


#define RUN_INTERVAL 5000
#define RUN_TIME 3000
int NUTRIENT_RUN_TIME = 2000;

#define STATE_MAIN 0
#define STATE_PUMP 1
#define STATE_NUTRIENT 2
#define STATE_WAIT 3
#define STATE_STIR 4
#define STATE_EMPTY 5

enum class DisplayState {
  CURRENT_EC,
  TEMP,
  WATER_LEVEL,
  TARGET_EC,
  INIT,
};

int state;
DisplayState dispState;
DisplayState prevDispState;

const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Conductivity conductivity(A5, A1, A2, A0);
Sonar sonar(A3, A4, 200);
Pump waterPump(9);
Pump stirPump(11);

int waterLevel = 0;  // keep track of water level (in terms of percentage filled?)


int targetECAdress = 0;
float targetEC = 0;

int screenState = 0;

unsigned long waitTimer = 0, pumpTimer = 0, interactionTimer = 0;

const int screenPin = 6, ecUpPin = 7, ecDownPin = 8;
OneButton btnScreen = OneButton(screenPin, true, true);
OneButton btnUp = OneButton(ecUpPin, true, true);
OneButton btnDown = OneButton(ecDownPin, true, true);

void setup() {
  Serial.begin(9600);
  float value = EEPROM.read(targetECAdress);
  Serial.println(value);
  if (value <= 254) targetEC = value / 10 + 0.1;  // for some reason it is always 0.1 too little
  else targetEC = 1.6;                            // Preferred value of EC is 1.6
  buttonSetup();
  conductivity.setup();
  conductivity.read();
  sonar.read();
  state = STATE_MAIN;
  dispState = DisplayState::INIT;
}

void loop() {
  btnScreen.tick();
  btnUp.tick();
  btnDown.tick();
  updateLCD();
  switch (state) {
    case STATE_MAIN:
      if (checkWaterLevel()) {
        waterPump.start();
        stirPump.start();
        pumpTimer = millis();
        state = STATE_PUMP;
      } else {
        waitTimer = millis();
        state = STATE_EMPTY;
      }
      break;
    case STATE_EMPTY:
      lcd.setCursor(0, 0);
      lcd.println("!! I AM EMPTY !!      ");
      
      if (millis() - waitTimer > RUN_INTERVAL) {
        state = STATE_MAIN;
      }
      break;
    case STATE_WAIT:
      lcd.setCursor(0, 0);
      lcd.println("IDLING...              ");

      if (millis() - waitTimer > RUN_INTERVAL) {
        state = STATE_MAIN;
      }
      break;
    case STATE_PUMP:
      lcd.setCursor(0, 0);
      lcd.println("PUMP AND STIR...  ");

      if (millis() - pumpTimer > RUN_TIME) {
        waterPump.stop();
        stirPump.stop();

        if (checkEC()) {
          waitTimer = millis();
          state = STATE_WAIT;
        } else {
          state = STATE_NUTRIENT;
        }
      }
      break;
    case STATE_NUTRIENT:
      lcd.setCursor(0, 0);
      lcd.print("PUMP NUTRIENTS...    ");
      // RUN NUTRIENTS FOR X AMOUNT OF TIME!!
      stirPump.start();
      pumpTimer = millis();
      state = STATE_STIR;
      break;
    case STATE_STIR:
      lcd.setCursor(0, 0);
      lcd.print("STIRRING...      ");
      if (millis() - pumpTimer > RUN_TIME) {
        stirPump.stop();
        state = STATE_MAIN;
      }
      break;
    default:
      Serial << "Uh oh.." << endl;
  }
}

long calculateRunTime() {
  int unitHeight = 15;
  int unitTime = 2000;
  return waterLevel * unitTime / unitHeight;
}

void updateLCD() {
  Serial << "DisplayState: " << static_cast<int>(dispState) << "Prev: " << static_cast<int>(prevDispState) << " time: " << millis() - interactionTimer << endl;
  switch (dispState) {
    case DisplayState::INIT:
      logoAnimation();
      interactionTimer = millis();
      dispState = DisplayState::CURRENT_EC;
      break;
    case DisplayState::CURRENT_EC:
      lcd.setCursor(0, 1);
      lcd << "EC: " << conductivity.EC25 << "                  ";
      break;
    case DisplayState::TEMP:
      lcd.setCursor(0, 1);
      lcd << "Temp: " << conductivity.Temperature << char(223) << "C              ";
      break;
    case DisplayState::WATER_LEVEL:
      lcd.setCursor(0, 1);
      lcd << "Water Lvl: " << waterLevel << "%                ";
      break;
    case DisplayState::TARGET_EC:
      lcd.setCursor(0, 1);
      lcd << "Target EC: " << targetEC << "     ";
      if (millis() - interactionTimer > 5000) {
        dispState = DisplayState::CURRENT_EC;
        saveEEPROM();
      }
      break;
    default:
      break;
  }
}

/**
* Check the water level. Return true if it's above the minimum height and false otherwise.
*/
bool checkWaterLevel() {
  unsigned int sonarDistance = sonar.read();
  float maxDistance = 10;  // distance to bottom TEMPORARY
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
  return conductivity.EC25 > targetEC;
}

void buttonSetup() {
  btnScreen.attachClick([]() {
    Serial.println("PRESSED BUTTON");
    interactionTimer = millis();
    goToNext();
  });
  btnUp.attachClick([]() {
    Serial.println("PRESSED BUTTON");
    changeProperty(true);
  });
  btnDown.attachClick([]() {
    Serial.println("PRESSED BUTTON");
    changeProperty(false);
  });
}

void saveEEPROM() {
  EEPROM.write(targetECAdress, targetEC * 10.0);
}

void changeProperty(bool up) {
  switch (dispState) {
    case DisplayState::TARGET_EC:
    case DisplayState::CURRENT_EC:
      if (up) {
        targetEC += 0.1;
      } else {
        targetEC -= 0.1;
      }
      dispState = DisplayState::TARGET_EC;
    default:
      break;
  }
  interactionTimer = millis();
}

void goToNext() {
  if (dispState == DisplayState::TARGET_EC) {
    saveEEPROM();
  }
  int nextState = static_cast<int>(dispState) + 1;
  if (nextState > static_cast<int>(DisplayState::WATER_LEVEL)) {
    dispState = DisplayState::CURRENT_EC;
  } else {
    dispState = static_cast<DisplayState>(nextState);
  }
}

void logoAnimation() {
  int animDelay = 80;
  int animDelay2 = 5;
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("H");
  delay(animDelay);
  lcd.print("Y");
  delay(animDelay);
  lcd.print("D");
  delay(animDelay);
  lcd.print("R");
  delay(animDelay);
  lcd.print("O");
  delay(animDelay);
  lcd.print("P");
  delay(animDelay);
  lcd.print("O");
  delay(animDelay);
  lcd.print("N");
  delay(animDelay);
  lcd.print("I");
  delay(animDelay);
  lcd.print("C");
  delay(500);

  lcd.setCursor(7, 1);
  lcd.print("S");
  delay(animDelay2);
  lcd.print("O");
  delay(animDelay2);
  lcd.print("L");
  delay(animDelay2);
  lcd.print("U");
  delay(animDelay2);
  lcd.print("T");
  delay(animDelay2);
  lcd.print("I");
  delay(animDelay2);
  lcd.print("O");
  delay(animDelay2);
  lcd.print("N");
  delay(animDelay2);
  lcd.print("S");
  delay(2000);

  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Version 0.1.1");
  delay(2000);
}
