#ifndef SERVOMOTOR_HPP
#define SERVOMOTOR_HPP

#include "NodeManager.hpp"
#include <M5Stack.h>

#define PWM_CHANNEL 0
#define PWM_FREQUENCY 50
#define PWM_RESOLUTION 16
#define SERVO_PIN 0

class ServoMotor {
public:
  ServoMotor();
  ~ServoMotor() = default;

  void initialize();
  void setAngle(int angle);
  int calculateAngle(const int warehouseId);

private:
  int currentAngle;
};

#endif
