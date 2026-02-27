#include "ServoMotor.hpp"
#include <Wire.h>

// Définitions pour le module GoPlus2
#define GOPLUS2_I2C_ADDR 0x38
#define SERVO_PORT 0

// On garde les angles précis du nouveau code pour éviter que le bras force
#define ANGLE_LEFT 93
#define ANGLE_CENTER 115
#define ANGLE_RIGHT 136
#define SERVO_SPEED_DELAY 15

ServoMotor::ServoMotor() { currentAngle = ANGLE_CENTER; }
void ServoMotor::initialize() {
  // On laisse juste un peu de temps au module externe pour s'allumer
  delay(200);

  // On met le bras au centre au démarrage.
  // Cela va appeler setAngle qui contient toute la logique Wire.
  setAngle(ANGLE_CENTER);
  Serial.println("Servo motor (GoPlus2) initialized via I2C");
}

void ServoMotor::setAngle(int targetAngle) {
  // 1. Sécurité des limites
  if (targetAngle < 0)
    targetAngle = 0;
  if (targetAngle > 180)
    targetAngle = 180;

  // 2. Si on est déjà au bon endroit, on ne fait rien
  if (currentAngle == targetAngle)
    return;

  // 3. Déterminer le sens de rotation (+1 pour monter, -1 pour descendre)
  int step = (targetAngle > currentAngle) ? 1 : -1;

  // 4. Mouvement fluide (Sweeping)
  while (currentAngle != targetAngle) {
    currentAngle += step; // On avance d'un degré

    // Transaction I2C pour ce petit pas
    Wire.beginTransmission(GOPLUS2_I2C_ADDR);
    Wire.write(SERVO_PORT);
    Wire.write((uint8_t)currentAngle);
    Wire.endTransmission();

    // Pause qui définit la vitesse visuelle du bras
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
