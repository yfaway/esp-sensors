#include "DigitalGasSensor.h"
#include <Arduino.h>

DigitalGasSensor::DigitalGasSensor(int pin, const std::string& topic,
        int aWarningThreshold) 
    : Sensor(pin, topic, -1, 2000), warningThreshold(aWarningThreshold)
{
    lastValue = -1;
}

int DigitalGasSensor::readValue() const {
    return digitalRead(pin);
}

bool DigitalGasSensor::isUpdatable(const int& currentValue) const {
    return currentValue != lastValue;
}

bool DigitalGasSensor::isInStableRange(const int& currentValue) const {
    return currentValue <= 130;
}

bool DigitalGasSensor::isInBadState(const int& currentValue) const {
    return currentValue >= warningThreshold;
}
