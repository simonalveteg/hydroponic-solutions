#ifndef SONAR_H
#define SONAR_H
#include <Arduino.h>
#include <NewPing.h>

class Sonar {
private:
  NewPing *sonar;
public:
  Sonar(int triggerPin, int echoPin, int maxDistance);
  unsigned int read();
  unsigned int ping_median(int interations);
};

#endif