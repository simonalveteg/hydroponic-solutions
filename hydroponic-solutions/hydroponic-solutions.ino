#include <LiquidCrystal.h>
#include <Streaming.h>
#include "Moisture.h"
#include "Sonar.h"
#include "Conductivity.h"
#include "Pump.h"

#define WATER_MIN_HEIGHT 5   // temporary
#define WATER_MAX_HEIGHT 15  // temporary

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Moisture moisture(A3, 13);
Sonar sonar(1, 0, 200);
Conductivity conductivity(8, A1, A2, A0);
Pump waterPump(6);
Pump nutrientPump(7);
Pump stirPump(9);

float waterState = 0;  // keep track of water level (in terms of percentage filled?)
float nutrientState = 0;

float ECsetPoint = 1.6;  // Preferred value of EC

void setup() {
  Serial.begin(9600);

  conductivity.setup();
  moisture.setup();

  // Start up the LCD screen
  lcd.begin(16, 2);
}

void loop() {
  Serial << "===== NEW READING ====" << endl;
  if (checkWaterLevel()) {
    checkMoisture();
    checkEC();
  }
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Water Level ");
  lcd.print(waterState);
  delay(1000);
}

/**
* Check the water level. Return true if it's above the minimum height and false otherwise.
*/
bool checkWaterLevel() {
  unsigned int sonarDistance = sonar.read();
  unsigned int waterHeight = WATER_MAX_HEIGHT - sonarDistance;
  waterState = waterHeight / WATER_MAX_HEIGHT;
  Serial << " Distance: " << sonarDistance << " cm." << endl;
  return waterHeight > WATER_MIN_HEIGHT;
}

/**
* Check moisture in grow-medium. If too low start water pump?
*/
void checkMoisture() {
  int moistureLevel = moisture.read();
  Serial << " Moisture Level: " << moistureLevel << "." << endl;
  int someLevel = 800;  // temporary
  if (moistureLevel < someLevel) {
    waterPump.start(5000);
  }
}

/**
* Check EC level. If EC is too low the nutrien-pump should run for a short time, then run the stir-pump for a while.
*/
void checkEC() {
  conductivity.read();
  conductivity.print();

  if (conductivity.EC25 < ECsetPoint) {
    nutrientPump.start(1000);
    stirPump.start(20000);
  }
}