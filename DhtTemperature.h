#ifndef DHT_TEMPERATURE_H
#define DHT_TEMPERATURE_H

#include <cstdint>
#include <string>

#include <AsyncMqttClient.h>
#include <DHT.h>

#include "Sensor.h"

/**
 * The processor for the DHT temperature sensors.
 */
class DhtTemperature : public Sensor<float, float>
{
    public:
        /**
         * Creates a new temperature processor for the sensor at the given pin.
         */
        DhtTemperature(int pin, DHT& dht, const std::string& topic,
                float threshold = 0.1);

    protected:
        float readValue() const;

    private:
        DHT& dht;
};

#endif
