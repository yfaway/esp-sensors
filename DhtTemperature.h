#ifndef DHT_TEMPERATURE_H
#define DHT_TEMPERATURE_H

#include <cstdint>

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
        DhtTemperature(int pin, float threshold = 0.1);

        /**
         * Called once by the sketch to set up this sensor.
         */
        void setup();

        void onProcessCycle(AsyncMqttClient& mqttClient,
                unsigned long currentTimeInMs);

    private:
        DHT dht;
};

#endif
