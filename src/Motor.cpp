
#include "Motor.hpp"

Motor::Motor() : stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN), isMotorPaused(true) {}

void Motor::initialize()
{
    stepper.setMaxSpeed(1000.0);
    stepper.setAcceleration(500.0);
    Serial.println("Motor initialized in absolute mode.");
}

void Motor::handleMotorInstructions(bool isProductReferenced, bool shouldMotorBeActive)
{
    if (isProductReferenced && shouldMotorBeActive) {
        if (isMotorPaused) {
            stepper.setSpeed(400.0);
            isMotorPaused = false;
        }
        stepper.runSpeed();
    } else {
        if (!isMotorPaused)
        {
        stepper.stop();
            isMotorPaused = true;
        }
        M5.Lcd.println("Motor is off");
    }
}

void Motor::setMotorPaused(bool paused)
{
    isMotorPaused = paused;
}

bool Motor::isPaused() const
{
    return isMotorPaused;
}