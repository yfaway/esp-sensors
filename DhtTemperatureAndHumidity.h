#ifndef DhtTemperatureAndHumidity_H
#define DhtTemperatureAndHumidity_H

#include <cstdint>
#include <string>

#include <AsyncMqttClient.h>
#include <DHT.h>

#include "DhtTemperature.h"
#include "DhtHumidity.h"

/**
 * The processor for the DHT temperature and humidity sensor.
 * There is a limitation on the DHT library, namely cannot use the multiple
 * DHT instances for the same pin (cause NAN value reading in the second 
 * instance). As such this class manages a single DHT instance, and dispatches
 * it to the individual DhtHumidity and DhtTemperature sensor. This allows
 * continue usage of the BaseSensor frame work.
 */
class DhtTemperatureAndHumidity : public BaseSensor
{
    public:
        /**
         * Creates a new temperature and humidity processor for the sensor at 
         * the given pin.
         */
        DhtTemperatureAndHumidity(int pin, const std::string& temperatureTopic,
                const std::string& humidityTopic);

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

    private:
        DHT dht;

        DhtTemperature temperature;
        DhtHumidity humidity;
};

#endif
