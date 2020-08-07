#include "AnalogGasSensor.h"
#include <Arduino.h>

AnalogGasSensor::AnalogGasSensor(
        int analogPin, const std::string& topic, int aThreshold) : 
    Sensor(analogPin, topic, aThreshold,
            [this] { return analogRead(this->pin); },
            this->absDifferenceThresholdFunction(),
            2000) 
{
    lastValue = -1;
}

