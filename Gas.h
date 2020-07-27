#ifndef GAS_H
#define GAS_H

#include <cstdint>
#include <AsyncMqttClient.h>

/**
 * Processor for the MQ gas sensor family.
 */
class Gas
{
    public:
        /**
         * Creates a new gas processor for the sensor at the given analog pin.
         */
        Gas(int analogPin);

        /**
         * Called once by the sketch to set up this sensor.
         */
        void setup();

        void onProcessCycle(AsyncMqttClient& mqttClient,
                unsigned long currentTimeInMs);

        /**
         * Called by the main sketch when it receives the MQTT ack.
         */
        void onMqttPacketAcknowledged(uint16_t packetId);

    public:
        /**
         * The threshold difference between the current value and the last
         * value, at which point an MQTT message will be sent out.
         */
        static const int DIFFERENCE_THRESHOLD;

    private:
        int pin;
        float lastValue;
        uint16_t sentPacketId;
        unsigned long mqttTimestampInMs; // last mqtt message
        unsigned long previousTimestampInMs;   // the last sensor reading timestamp
};

#endif
