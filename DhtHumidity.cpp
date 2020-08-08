#include "DhtHumidity.h"
#include <Arduino.h>

DhtHumidity::DhtHumidity(
        int pin, DHT& aDht, const std::string& topic, float threshold) : 
    Sensor(pin, topic, threshold),
    dht(aDht) {
}

float DhtHumidity::readValue() const {
    return dht.readHumidity();
}
