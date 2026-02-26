
// #include "Motor.hpp"

// Motor::Motor() : stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN), isMotorPaused(true) {}

// void Motor::initialize()
// {
//     stepper.setMaxSpeed(1000.0);
//     stepper.setAcceleration(500.0);
//     Serial.println("Motor initialized in absolute mode.");
// }

// void Motor::handleMotorInstructions(bool isProductReferenced, bool shouldMotorBeActive)
// {
//     if (isProductReferenced && shouldMotorBeActive) {
//         if (isMotorPaused) {
//             stepper.setSpeed(400.0);
//             isMotorPaused = false;
//         }
//         stepper.runSpeed();
//     } else {
//         if (!isMotorPaused)
//         {
//         stepper.stop();
//             isMotorPaused = true;
//         }
//         M5.Lcd.println("Motor is off");
//     }
// }

// void Motor::setMotorPaused(bool paused)
// {
//     isMotorPaused = paused;
// }

// bool Motor::isPaused() const
// {
//     return isMotorPaused;
// }


#include "Motor.hpp"

Motor::Motor() : grbl(STEPMOTOR_I2C_ADDR), isMotorPaused(true) {}

void Motor::initialize()
{
    Wire.begin(21, 22);
    grbl.Init(&Wire);
    grbl.setMode("absolute");
    Serial.println("Motor initialized in absolute mode.");
}

void Motor::handleMotorInstructions(bool isProductReferenced, bool shouldMotorBeActive)
{
    if (isProductReferenced && shouldMotorBeActive) {
        if (isMotorPaused) {
            grbl.sendGcode(STARTING_MOTOR_AFTER_A_STOP);
            isMotorPaused = false;
        }
            grbl.sendGcode(RELATIVE_MODE);
        grbl.setMotor(20, 20, 20, 200);
    } else {
        if (!isMotorPaused)
        {
            grbl.sendGcode(STOPPING_MOTOR);
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
