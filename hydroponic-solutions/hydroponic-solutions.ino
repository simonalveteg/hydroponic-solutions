#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

#define VIN 5

int R1 = 1000;
int Ra = 25;  //Resitance of Digital Pin, 25 ohms for mega/uno
int ECPin = A0;
int ECPower = A1;
int ECGround = A2;

float TemperatureCoef = 0.019;  //this changes depending on what chemical we are measuring
float PPMconversion = 0.64;

float K = 1.76;  // Cell constant for EU plug

#define ONE_WIRE_BUS 8  // Data wire For Temp Probe is plugged into pin 0 on the Arduino

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/*
 * The setup function. We only start the sensors here
 */
void setup(void) {
  R1 = R1 + Ra;  // Add internal digital pin resistance
  pinMode(ECPin, INPUT);
  pinMode(ECPower, OUTPUT);     //Setting pin for sourcing current
  pinMode(ECGround, OUTPUT);    //setting pin for sinking current
  digitalWrite(ECGround, LOW);  //We can leave the ground connected permanantly

  Serial.begin(9600);
  // Start up the dallas temperature library
  sensors.begin();
  // Start up the LCD screen
  lcd.begin(16, 2);
}

void loop(void) {
  lcd.setCursor(0, 0);
  float temperature = readTemperature();
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print("PPM: ");
  lcd.print(readPPM(temperature));
}

float readTemperature() {
  // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();  // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);
    return tempC;
  } else {
    Serial.println("Error: Could not read temperature data");
  }
}

float readEC() {
  //************Estimates Resistance of Liquid ****************//
  digitalWrite(ECPower, HIGH);
  float raw = analogRead(ECPin);
  digitalWrite(ECPower, LOW);
  //***************** Converts to EC **************************//
  float Vdrop = (VIN * raw) / 1024.0;
  float Rc = (Vdrop * R1) / (VIN - Vdrop);
  Rc = Rc - Ra;
  float EC = 1000 / (Rc * K);
  return EC;
}

float readPPM(float temperature) {
  float EC = readEC();
  //*************Compensating For Temperaure********************//
  float EC25 = EC / (1 + TemperatureCoef * (temperature - 25.0));
  float ppm = (EC25) * (PPMconversion * 1000);
  return ppm;
}