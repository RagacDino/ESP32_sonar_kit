#include "Components.h"

Components::Components(int servoPin, int maxDistanceCm) : _servoPin(servoPin), _maxDistanceCm(maxDistanceCm) {}

void Components::begin() {
    _myServo.attach(_servoPin, 500, 2400); // Standard servo pulse widths
    _laserSensor.setTimeout(500);
    _laserSensor.begin();

    _laserSensor.setDistanceMode(VL53L1X::Long);
    _laserSensor.setMeasurementTimingBudget(50000);

    // Start continuous readings at a rate of one measurement every 50 ms (the
    // inter-measurement period). This period should be at least as long as the
    // timing budget.
    _laserSensor.startContinuous(50);
}

int Components::getDistance() {
  int distance_in_mm = _laserSensor.read(); // VL53L1X returns distance in 'mm'
  if (_laserSensor.timeoutOccurred()) { // Check if sensor has been measuring longer than timeout period
    Serial.println(" LASER SENSOR TIMEOUT");
  }
  return distance_in_mm / 10;
}

void Components::setServoAngle(int angle) {
    _myServo.write(180 - angle); // Servo angle matches scan angle (0-180)
}