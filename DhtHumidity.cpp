#include "DhtHumidity.h"
#include <Arduino.h>

DhtHumidity::DhtHumidity(
        int pin, DHT& aDht, const std::string& topic, float threshold) : 
    Sensor(pin, topic, threshold,
            [this] { return dht.readHumidity(); },
            this->absDifferenceThresholdFunction(),
            10000, 120000),
    dht(aDht) {
}
