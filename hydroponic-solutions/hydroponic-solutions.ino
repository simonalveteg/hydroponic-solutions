
int sensorPin = A2;   // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int sensorVCC = 13;

void setup() {
  Serial.begin(9600);
  Serial.println("SETUP");
  pinMode(sensorVCC, OUTPUT);  // declare the ledPin as an OUTPUT:
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorVCC, LOW);
}

void loop() {
  Serial.println("HEJ?");
  digitalWrite(sensorVCC, HIGH);        // power the sensor
  delay(100);                           //make sure the sensor is powered
  sensorValue = analogRead(sensorPin);  // read the value from the sensor:
  digitalWrite(sensorVCC, LOW);         //stop power
  delay(60 * 1000);                     //wait
  Serial.println(sensorValue);
  Serial.println("Hej igen");
}

void bla() {

  digitalWrite(sensorVCC, HIGH);        // power the sensor
  delay(100);                           //make sure the sensor is powered
  sensorValue = analogRead(sensorPin);  // read the value from the sensor:
  digitalWrite(sensorVCC, LOW);         //stop power
  delay(60 * 1000);                     //wait
  Serial.print("sensor = ");
  Serial.println(sensorValue);
}

void pwmWrite(uint8_t pin, float percent) {
  int value = 255 * percent;
  analogWrite(pin, value);
}
