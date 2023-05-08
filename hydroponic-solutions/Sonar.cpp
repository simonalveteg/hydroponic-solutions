#include "Sonar.h"

Sonar::Sonar(int triggerPin, int echoPin, int maxDistance) {
  sonar = new NewPing(triggerPin, echoPin, maxDistance);
}
unsigned int Sonar::read() {
  return sonar->ping_cm();
}
unsigned int Sonar::ping_median(int iterations) {
  return sonar->ping_median(iterations, 200);
}