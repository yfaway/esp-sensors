#ifndef SENSOR_H
#define SENSOR_H

#include <cstdint>
#include <AsyncMqttClient.h>

/**
 * The base of all sensor types.
 */
template <typename ValueType, typename ThresholdType>
class Sensor
{
    public:
        /**
         * Creates a new gas processor for the sensor at the given analog pin.
         */
        Sensor(int aPin, ThresholdType aThreshold = -1) :
            pin(aPin), threshold(aThreshold),
            lastValue(), sentPacketId(0), mqttTimestampInMs(0),
            previousTimestampInMs(0) {
        }

        /**
         * Called once by the sketch to set up this sensor.
         */
        virtual void setup() {
        }

        virtual void onProcessCycle(AsyncMqttClient& mqttClient,
                unsigned long currentTimeInMs) = 0;

        /**
         * Called by the main sketch when it receives the MQTT ack.
         */
        virtual void onMqttPacketAcknowledged(uint16_t packetId) {
            if ( packetId == sentPacketId ) {
                mqttTimestampInMs = millis();
            }
        }

    protected: // input values
        int pin;
        ThresholdType threshold;

    protected: // state values
        ValueType lastValue;
        uint16_t sentPacketId;
        unsigned long mqttTimestampInMs; // last mqtt message
        unsigned long previousTimestampInMs;   // the last sensor reading timestamp
};

#endif
