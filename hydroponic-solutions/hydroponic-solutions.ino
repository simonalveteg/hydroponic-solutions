#include <LiquidCrystal.h>
#include "Moisture.h"
#include "Sonar.h"
#include "Conductivity.h"

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Moisture moisture(A3, 13);
Sonar sonar(1, 0, 200);
Conductivity conductivity(8, A1, A2, A0);

void setup() {
  Serial.begin(9600);

  conductivity.setup();
  moisture.setup();

  // Start up the LCD screen
  lcd.begin(16, 2);
}

void loop() {

  int moistureLevel = moisture.read();
  Serial.print("Moisture Level: ");
  Serial.println(moistureLevel);

  unsigned int sonarDistance = sonar.read();  // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping: ");
  Serial.print(sonarDistance);  // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  Serial.println("cm");

  conductivity.read();
  conductivity.print();

  delay(1000);
}
