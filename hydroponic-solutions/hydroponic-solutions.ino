
const int PWM_PIN = 10;
const int ANAL_PIN = A5;

void setup() {
  pinMode(PWM_PIN, OUTPUT);
  pinMode(ANAL_PIN, OUTPUT);
}

void loop() {
  for (int i = 255; i > 0; i--) {
    analogWrite(PWM_PIN, i);
    Serial.println(i);
    delay(500);
    i -= 9;
  }
}
