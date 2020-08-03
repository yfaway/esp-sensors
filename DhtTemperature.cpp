#include "DhtTemperature.h"
#include <Arduino.h>

DhtTemperature::DhtTemperature(
        int pin, DHT& aDht, const std::string& topic, float threshold) : 
    Sensor(pin, topic, threshold, [this] { return dht.readTemperature(); },
            this->absDifferenceThresholdFunction()),
    dht(aDht) {
}
