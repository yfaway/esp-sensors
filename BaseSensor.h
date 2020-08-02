#ifndef BASE_SENSOR_H
#define BASE_SENSOR_H

#include <cstdint>
#include <AsyncMqttClient.h>

/**
 * The base of all sensor types.
 */
class BaseSensor
{
    public:
        /**
         * Called once by the sketch to set up this sensor.
         */
        virtual void setup() = 0;

        virtual void onProcessCycle(AsyncMqttClient& mqttClient,
                unsigned long currentTimeInMs) = 0;

        /**
         * Called by the main sketch when it receives the MQTT ack.
         */
        virtual void onMqttPacketAcknowledged(uint16_t packetId) = 0;
};

#endif
