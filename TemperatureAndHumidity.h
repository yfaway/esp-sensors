#ifndef TemperatureAndHumidity_H
#define TemperatureAndHumidity_H

#include <cstdint>

#include <AsyncMqttClient.h>
#include "DHT.h"

/**
 * The processor for the DHT temperature and humidity sensors.
 */
class TemperatureAndHumidity
{
    public:
        /**
         * Creates a new temperature and humidity processor for the sensor at 
         * the given pin.
         */
        TemperatureAndHumidity(int pin);

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
        static const float DIFFERENCE_THRESHOLD;

    private:
        DHT dht;

        float temperature;
        float humidity;

        uint16_t temperaturePacketId;
        uint16_t humidityPacketId;

        unsigned long mqttTemperatureTimestampInMs; // last mqtt temp message
        unsigned long mqttHumidityTimestampInMs; // last mqtt humidity message

        unsigned long previousTimestampInMs;   // the last sensor reading timestamp
};

#endif
