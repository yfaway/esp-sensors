#include "DhtTemperature.h"
#include <Arduino.h>

DhtTemperature::DhtTemperature(
        int pin, DHT& aDht, const std::string& topic, float threshold) : 
    Sensor(pin, topic, threshold),
    dht(aDht) {
}

float DhtTemperature::readValue() const {
    return dht.readTemperature();
}
