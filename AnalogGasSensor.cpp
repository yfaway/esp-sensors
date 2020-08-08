#include "AnalogGasSensor.h"
#include <Arduino.h>

AnalogGasSensor::AnalogGasSensor(
        int analogPin, const std::string& topic, int aWarningThreshold,
        int aThreshold) : 
    Sensor(analogPin, topic, aThreshold, 2000),
    warningThreshold(aWarningThreshold) 
{
    lastValue = -1;
}

int AnalogGasSensor::readValue() const {
    return analogRead(this->pin);
}


bool AnalogGasSensor::isInStableRange(const int& currentValue) const {
    return currentValue <= 130;
}

bool AnalogGasSensor::isInBadState(const int& currentValue) const {
    return currentValue >= warningThreshold;
}
