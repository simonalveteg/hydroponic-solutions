#define STEP_PIN 12
#define DIR_PIN 13
#define ENABLE_PIN 11
#define TIME 750

void setup() {
  // put your setup code here, to run once:
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("hakfsjdh");
  for (int i = 0; i < 360; i++) { // gjorde en loop av nån anledning
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(TIME);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(TIME);
  }
}
