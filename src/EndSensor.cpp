#include "EndSensor.hpp"
#include "HardwareSerial.h"

EndSensor::EndSensor() {}

void EndSensor::initialize()
{
    pinMode(ENDSENSOR_READ_PIN, INPUT);
    Serial.println("EndSensor initialized.");
}

bool EndSensor::isEndSensorDetected() const
{
    return digitalRead(ENDSENSOR_READ_PIN) == HIGH;
}