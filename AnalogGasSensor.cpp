#include "AnalogGasSensor.h"
#include <Arduino.h>

AnalogGasSensor::AnalogGasSensor(
        int analogPin, const std::string& topic, int aThreshold) : 
    Sensor(analogPin, topic, aThreshold,
            [&analogPin] { return analogRead(analogPin); }) {
}

