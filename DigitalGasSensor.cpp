#include "DigitalGasSensor.h"
#include <Arduino.h>

DigitalGasSensor::DigitalGasSensor(int pin, const std::string& topic) 
    : Sensor(pin, topic, -1, [this] { return digitalRead(13); },
            this->simpleDifferenceThresholdFunction(),
            2000)
{
    lastValue = -1;
}
