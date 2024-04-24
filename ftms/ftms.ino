#include <Arduino.h>

uint16_t speed = 10;   // Initial speed in km/h
uint16_t cadence = 50; // Initial cadence in RPM
uint16_t power = 100;  // Initial power in watts

void setup() {
  Serial.begin(9600);
  setupBLE();
}

void loop() {
  handleBLE(speed, cadence, power);

  // Incrementing values for demonstration
  speed += 1;
  cadence += 1;
  power += 2;

  delay(2000); // Data update interval
}