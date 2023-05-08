#include <NewPing.h>
#define TRIGGER_PIN  3  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(29);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  float cmSensor = sonar.ping_median(100)/58.2; // Send ping, get ping time in microseconds (uS).
  float cmWaterheight = 12-cmSensor;
  if
  Serial.print("Ping: ");
  Serial.print(cmWaterheight); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  Serial.println("cm");
}
