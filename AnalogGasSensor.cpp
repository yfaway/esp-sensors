#include "AnalogGasSensor.h"
#include <Arduino.h>

const int BAD_STATE_THRESHOLD = 2;

AnalogGasSensor::AnalogGasSensor(
        int analogPin, const std::string& topic, int aWarningThreshold,
        int aThreshold) : 
    Sensor(analogPin, topic, aThreshold, 2000),
    warningThreshold(aWarningThreshold),
    badStateCount(0)
{
    lastValue = -1;
}

int AnalogGasSensor::readValue() const {
    return analogRead(this->pin);
}

bool AnalogGasSensor::isInStableRange(const int& currentValue) const {
    return currentValue <= 130;
}

bool AnalogGasSensor::isInBadState(const int& currentValue) {
    if ( currentValue >= warningThreshold ) {
        badStateCount++;
        if ( badStateCount >= BAD_STATE_THRESHOLD ) {
            return true;
        }
        return false;
    }
    else {
        badStateCount = 0;
        return false;
    }
}
