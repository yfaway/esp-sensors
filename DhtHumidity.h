#ifndef DHT_HUMIDITY_H
#define DHT_HUMIDITY_H

#include <cstdint>

#include <AsyncMqttClient.h>
#include <DHT.h>

#include "Sensor.h"

/**
 * The processor for the DHT temperature sensors.
 */
class DhtHumidity : public Sensor<float, float>
{
    public:
        /**
         * Creates a new temperature processor for the sensor at the given pin.
         */
        DhtHumidity(int pin, DHT& dht, float threshold = 0.1);

        void onProcessCycle(AsyncMqttClient& mqttClient,
                unsigned long currentTimeInMs);

    private:
        DHT& dht;
};

#endif
