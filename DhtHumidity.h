#ifndef DHT_HUMIDITY_H
#define DHT_HUMIDITY_H

#include <string>
#include <DHT.h>
#include "Sensor.h"

/**
 * The humidity processor for the DHT temperature sensors.
 */
class DhtHumidity : public Sensor<float, float>
{
    public:
        /**
         * Creates a new temperature processor for the sensor at the given pin.
         */
        DhtHumidity(int pin, DHT& dht, const std::string& topic, float threshold = 0.1);

    protected:
        float readValue() const;

    private:
        DHT& dht;
};

#endif
