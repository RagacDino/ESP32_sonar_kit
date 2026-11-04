/*
  Components class handles the control of the servo motor and the distance
  measurement using a VL53L1X time-of-flight laser sensor. 
  
  It provides methods for initialization, setting the servo angle, and 
  retrieving distance measurements. 
*/

#ifndef Components_h
#define Components_h

#include <Arduino.h>
#include <ESP32_Servo.h>
#include "VL53L1X-SOLDERED.h"

class Components {
  public:
    Components(int servoPin, int maxDistanceCm);
    void begin();
    int getDistance();
    void setServoAngle(int angle);
    int getMaxDistance();

  private:
    int _servoPin;
    int _maxDistanceCm;
    Servo _myServo;
    VL53_L1X _laserSensor;
};

#endif