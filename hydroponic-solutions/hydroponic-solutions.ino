#include <LiquidCrystal.h>
#include <Streaming.h>
#include <OneButton.h>
#include <EEPROM.h>
#include "Sonar.h"
#include "Nutrient.h"
#include "Pump.h"


#define WAIT_TIME 600000
#define RUN_TIME 600000
#define STIR_TIME 30000

#define STATE_MAIN 0
#define STATE_PUMP 1
#define STATE_NUTRIENT 2
#define STATE_WAIT 3
#define STATE_STIR 4
#define STATE_EMPTY 5

enum class DisplayState {
  CURRENT_CONC,
  TEMP,
  WATER_LEVEL,
  PUMPED,
  TARGET_CONC,
  INIT,
};

int state;
DisplayState dispState;
DisplayState prevDispState;

const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

Nutrient nutrient(A5, A1, A2, A0, 13);
Sonar sonar(A3, A4, 200);
Pump waterPump(9);
Pump stirPump(11);

float waterLevel = 0;     // keep track of water level (in terms of percentage filled?)
float cmWaterheight = 0;  // keep track of water height in cm

int targetConcAdress = 0;
float targetConc = 0;

int nutrientAddedAdress = 8;
float nutrientAdded = 0;

int screenState = 0;

unsigned long waitTimer = 0, pumpTimer = 0, interactionTimer = 0;

const int screenPin = 6, ecUpPin = 7, ecDownPin = 8;
OneButton btnScreen = OneButton(screenPin, true, true);
OneButton btnUp = OneButton(ecUpPin, true, true);
OneButton btnDown = OneButton(ecDownPin, true, true);


void setup() {
  Serial.begin(9600);
  float value = EEPROM.read(targetConcAdress);
  Serial.println(value);
  if (value <= 254) targetConc = value / 10 + 0.1;  // for some reason it is always 0.1 too little
  else targetConc = 2.0;                            // Preferred concentration is 2.0 ml/l

  float val = EEPROM.read(nutrientAddedAdress);
  Serial.println(val);
  if (val <= 254) nutrientAdded = val / 10 + 0.1;  // for some reason it is always 0.1 too little
  else nutrientAdded = 0.0;                        // Preferred concentration is 2.0 ml/l


  buttonSetup();
  nutrient.setup();
  nutrient.read();
  sonar.read();
  state = STATE_MAIN;
  dispState = DisplayState::INIT;
}

void loop() {
  Serial.println(state);
  btnScreen.tick();
  btnUp.tick();
  btnDown.tick();
  updateLCD();
  switch (state) {
    case STATE_MAIN:
      Serial.println("MAIN??");
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
      Serial.println("EMPTY??");
      lcd.setCursor(0, 0);
      lcd.println("!! I AM EMPTY !!      ");

      if (millis() - waitTimer > WAIT_TIME) {
        state = STATE_MAIN;
      }
      break;
    case STATE_WAIT:
      Serial.println("WAIT??");
      lcd.setCursor(0, 0);
      lcd.println("IDLING...              ");

      if (millis() - waitTimer > WAIT_TIME) {
        state = STATE_MAIN;
      }
      break;
    case STATE_PUMP:
      Serial.println("PUMP??");
      lcd.setCursor(0, 0);
      lcd.println("PUMP AND STIR...  ");

      if (millis() - pumpTimer > RUN_TIME) {
        waterPump.stop();
        stirPump.stop();

        if (checkConcentration()) {
          waitTimer = millis();
          state = STATE_WAIT;
        } else {
          state = STATE_NUTRIENT;
        }
      }
      break;
    case STATE_NUTRIENT:
      Serial.println("NUTRIENT??");
      lcd.setCursor(0, 0);
      lcd.print("PUMP NUTRIENTS...    ");
      nutrientAdded += nutrient.refill(getWaterVolume(), targetConc);
      EEPROM.write(nutrientAddedAdress, nutrientAdded * 10.0);
      Serial.println("h");
      stirPump.start();
      Serial.println("he");
      pumpTimer = millis();
      Serial.println("hej");
      state = STATE_STIR;
      Serial.println("hejs");
      break;
    case STATE_STIR:
      Serial.println("STIR??");
      Serial.println("g");
      lcd.setCursor(0, 0);
      Serial.println("ge");
      lcd.print("STIRRING...      ");
      if (millis() - pumpTimer > RUN_TIME) {
        stirPump.stop();
        Serial.println("gej");
        state = STATE_MAIN;
      }
      break;
    default:
      Serial << "Uh oh.." << endl;
  }
}

void updateLCD() {
  // Serial << "DisplayState: " << static_cast<int>(dispState) << "Prev: " << static_cast<int>(prevDispState) << " time: " << millis() - interactionTimer << endl;
  switch (dispState) {
    case DisplayState::INIT:
      logoAnimation();
      interactionTimer = millis();
      dispState = DisplayState::CURRENT_CONC;
      break;
    case DisplayState::CURRENT_CONC:
      lcd.setCursor(0, 1);
      lcd << "Conc: " << nutrient.concentration << "ml/l                  ";
      break;
    case DisplayState::TEMP:
      lcd.setCursor(0, 1);
      lcd << "Temp: " << nutrient.temperature << char(223) << "C              ";
      break;
    case DisplayState::WATER_LEVEL:
      lcd.setCursor(0, 1);
      lcd << "W: " << waterLevel << "%, " << getWaterVolume() << "l                ";
      break;
    case DisplayState::TARGET_CONC:
      lcd.setCursor(0, 1);
      lcd << "Target conc: " << targetConc << "     ";
      if (millis() - interactionTimer > 5000) {
        dispState = DisplayState::CURRENT_CONC;
        saveEEPROM();
      }
      break;
    case DisplayState::PUMPED:
      lcd.setCursor(0, 1);
      lcd << "Pumped: " << nutrientAdded << " ml    ";
      break;
    default:
      break;
  }
}

/**
* Check the water level. Return true if it's above the minimum height and false otherwise.
*/
bool checkWaterLevel() {
  int maxDistance = 12;
  int minDistance = 4;
  float cmSensor = sonar.ping_median(20) / 58.2;
  cmWaterheight = maxDistance - cmSensor;
  waterLevel = 100 - 100 * (cmSensor - minDistance) / (maxDistance - minDistance);
  waterLevel = (waterLevel < 0) ? 0 : waterLevel;
  waterLevel = ((waterLevel > 100) ? 100 : waterLevel);
  Serial << "Water height: " << cmWaterheight << "cm, Water level: " << waterLevel << "%, "
         << "Water volume: " << getWaterVolume() << " litres" << endl;
  return cmSensor >= minDistance && cmSensor <= maxDistance;
}

float getWaterVolume() {
  return 0.385 * cmWaterheight;
}

/**
* Check concentration. Return true if value is above setpoint.
*/
bool checkConcentration() {
  nutrient.read();
  nutrient.print();
  return nutrient.concentration > targetConc * 0.9;
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
  EEPROM.write(targetConcAdress, targetConc * 10.0);
}

void changeProperty(bool up) {
  switch (dispState) {
    case DisplayState::TARGET_CONC:
    case DisplayState::CURRENT_CONC:
      if (up) {
        targetConc += 0.1;
      } else {
        targetConc -= 0.1;
      }
      dispState = DisplayState::TARGET_CONC;
    default:
      break;
  }
  interactionTimer = millis();
}

void goToNext() {
  if (dispState == DisplayState::TARGET_CONC) {
    saveEEPROM();
  }
  int nextState = static_cast<int>(dispState) + 1;
  if (nextState > static_cast<int>(DisplayState::PUMPED)) {
    dispState = DisplayState::CURRENT_CONC;
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
