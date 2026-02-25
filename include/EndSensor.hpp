#ifndef ENDSENSOR_HPP
#define ENDSENSOR_HPP

#include <Arduino.h>

#define ENDSENSOR_READ_PIN 36   //A modif

class EndSensor
{
public:
    EndSensor();

    void initialize();
    bool isEndSensorDetected() const;
};

#endif