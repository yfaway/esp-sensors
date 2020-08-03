#include "DigitalGasSensor.h"
#include <Arduino.h>

DigitalGasSensor::DigitalGasSensor(int pin, const std::string& topic) 
    : Sensor(pin, topic, -1, [&pin] { return digitalRead(pin); },
            this->simpleDifferenceThresholdFunction())
{
}
