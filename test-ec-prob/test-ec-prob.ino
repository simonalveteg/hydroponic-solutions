
#include <OneWire.h>
#include <DallasTemperature.h>


int R1 = 1000;
int Ra = 25;  //Resistance of powering Pins
int ECPin = A1;
int ECGround = A2;
int ECPower = A0;
float PPMconversion = 0.7;
float TemperatureCoef = 0.019;  //this changes depending on what chemical we are measuring
float K = 1.67;
#define ONE_WIRE_BUS 8             // Data wire For Temp Probe is plugged into pin 10 on the Arduino
const int TempProbePossitive = 9;  //Temp Probe power connected to pin 9
const int TempProbeNegative = 7;   //Temp Probe Negative connected to pin 8


OneWire oneWire(ONE_WIRE_BUS);        // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.

float Temperature = 10;
float EC = 0;
float EC25 = 0;
int ppm = 0;


float raw = 0;
float Vin = 5;
float Vdrop = 0;
float Rc = 0;
float buffer = 0;



void setup() {
  Serial.begin(9600);
  pinMode(TempProbeNegative, OUTPUT);    //seting ground pin as output for tmp probe
  digitalWrite(TempProbeNegative, LOW);  //Seting it to ground so it can sink current
  pinMode(TempProbePossitive, OUTPUT);   //ditto but for positive
  digitalWrite(TempProbePossitive, HIGH);
  pinMode(ECPin, INPUT);
  pinMode(ECPower, OUTPUT);     //Setting pin for sourcing current
  pinMode(ECGround, OUTPUT);    //setting pin for sinking current
  digitalWrite(ECGround, LOW);  //We can leave the ground connected permanantly

  delay(100);  // gives sensor time to settle
  sensors.begin();
  delay(100);
  //** Adding Digital Pin Resistance to [25 ohm] to the static Resistor *********//
  // Consule Read-Me for Why, or just accept it as true
  R1 = (R1 + Ra);  // Taking into acount Powering Pin Resitance

  Serial.println("ElCheapo Arduino EC-PPM measurments");
  Serial.println("By: Michael Ratcliffe  Mike@MichaelRatcliffe.com");
  Serial.println("Free software: you can redistribute it and/or modify it under GNU ");
  Serial.println("");
  Serial.println("Make sure Probe and Temp Sensor are in Solution and solution is well mixed");
  Serial.println("");
  Serial.println("Measurments at 5's Second intervals [Dont read Ec morre than once every 5 seconds]:");
}

void loop() {
  GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
  PrintReadings();  // Cals Print routine [below main loop]
  delay(1000);
}

void GetEC() {
  sensors.requestTemperatures();             // Send the command to get temperatures
  Temperature = sensors.getTempCByIndex(0);  //Stores Value in Variable

  digitalWrite(ECPower, HIGH);
  raw = analogRead(ECPin);
  raw = analogRead(ECPin);  // This is not a mistake, First reading will be low beause if charged a capacitor
  digitalWrite(ECPower, LOW);

  //***************** Converts to EC **************************//
  Vdrop = (Vin * raw) / 1024.0;
  Rc = (Vdrop * R1) / (Vin - Vdrop);
  Rc = Rc - Ra;  //acounting for Digital Pin Resitance
  EC = 1000 / (Rc * K);


  //*************Compensating For Temperaure********************//
  EC25 = EC / (1 + TemperatureCoef * (Temperature - 25.0));
  ppm = (EC25) * (PPMconversion * 1000);
}


//***This Loop Is called From Main Loop- Prints to serial usefull info ***//
void PrintReadings() {
  Serial.print("Rc: ");
  Serial.print(Rc);
  Serial.print(" EC: ");
  Serial.print(EC25);
  Serial.print(" Siemens  ");
  Serial.print(ppm);
  Serial.print(" ppm  ");
  Serial.print(Temperature);
  Serial.println(" *C ");
}