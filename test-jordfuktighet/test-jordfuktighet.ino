
int sensorPin = A2;   // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int sensorVCC = 13;

void setup() {
  Serial.begin(9600);
  pinMode(sensorVCC, OUTPUT);  // declare the ledPin as an OUTPUT:
  digitalWrite(sensorVCC, LOW);
}

void loop() {
  digitalWrite(sensorVCC, HIGH);        // power the sensor
  delay(100);                           //make sure the sensor is powered
  sensorValue = analogRead(sensorPin);  // read the value from the sensor:
  digitalWrite(sensorVCC, LOW);         //stop power
  delay(1000);                     //wait
  Serial.println(sensorValue);
}
