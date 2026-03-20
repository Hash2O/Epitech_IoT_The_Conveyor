#include "Motor.hpp"

Motor::Motor() : grbl(STEPMOTOR_I2C_ADDR), isMotorPaused(true) {}

void Motor::initialize() {
  grbl.Init();
  delay(500);

  grbl.sendGcode((char *)"$X");
  delay(100);

  grbl.sendGcode((char *)"$111=500");
  delay(50);
  grbl.sendGcode((char *)"$121=50");

  delay(50);
  // ------------------------------

  grbl.sendGcode((char *)"G91");
  delay(100);

  grbl.sendGcode((char *)"M3 S1000");
  delay(100);
}
void Motor::handleMotorInstructions(bool isProductReferenced,
                                    bool shouldMotorBeActive) {
  if (shouldMotorBeActive) {
    if (isMotorPaused) {
      grbl.sendGcode((char *)"~");
      delay(50);

      grbl.sendGcode((char *)"G1 Y10000 F150");

      isMotorPaused = false;
      M5.Lcd.println("Moteur: MARCHE FORCEE");
    }
  } else {
    if (!isMotorPaused) {
      grbl.sendGcode((char *)"!");
      isMotorPaused = true;
      M5.Lcd.println("Moteur: ARRET");
    }
  }
}
void Motor::setMotorPaused(bool paused) { isMotorPaused = paused; }

bool Motor::isPaused() const { return isMotorPaused; }
