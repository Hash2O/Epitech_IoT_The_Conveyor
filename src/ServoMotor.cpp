#include "ServoMotor.hpp"
#include <Wire.h>

#define GOPLUS2_I2C_ADDR 0x38
#define SERVO_PORT 0

#define ANGLE_LEFT 75
#define ANGLE_CENTER 91
#define ANGLE_RIGHT 115
#define SERVO_SPEED_DELAY 15

ServoMotor::ServoMotor() { currentAngle = ANGLE_CENTER; }
void ServoMotor::initialize() {
  delay(200);

  setAngle(ANGLE_CENTER);
  Serial.println("Servo motor (GoPlus2) initialized via I2C");
}

void ServoMotor::setAngle(int targetAngle) {
  if (targetAngle < 0)
    targetAngle = 0;
  if (targetAngle > 180)
    targetAngle = 180;

  if (currentAngle == targetAngle)
    return;

  int step = (targetAngle > currentAngle) ? 1 : -1;

  while (currentAngle != targetAngle) {
    currentAngle += step;

    Wire.beginTransmission(GOPLUS2_I2C_ADDR);
    Wire.write(SERVO_PORT);
    Wire.write((uint8_t)currentAngle);
    Wire.endTransmission();

    delay(SERVO_SPEED_DELAY);
  }
}
int ServoMotor::calculateAngle(const int warehouseId) {
  switch (warehouseId) {
  case 1:
    return ANGLE_LEFT;
  case 2:
    return ANGLE_CENTER;
  case 3:
    return ANGLE_RIGHT;
  default:
    return ANGLE_CENTER;
  }
}
