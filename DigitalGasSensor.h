#ifndef DIGITAL_GAS_H
#define DIGITAL_GAS_H

#include <cstdint>
#include <AsyncMqttClient.h>

#include "Sensor.h"

/**
 * Processor for the MQ gas sensor family using the digital pin.
 */
class DigitalGasSensor : public Sensor<int, int>
{
    public:
        /**
         * Creates a new gas processor for the sensor at the given digital pin.
         */
        DigitalGasSensor(int digitalPin);

        void onProcessCycle(AsyncMqttClient& mqttClient,
                unsigned long currentTimeInMs);
};

#endif
