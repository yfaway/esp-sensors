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
    protected:
        /**
         * Creates a new temperature and humidity processor for the sensor at 
         * the given pin.
         */
        DhtTemperatureAndHumidity(int dhtType, int pin, const std::string& temperatureTopic,
                const std::string& humidityTopic);

    public:
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

class Dht11TemperatureAndHumidity : public DhtTemperatureAndHumidity 
{
    public:
        /**
         * Creates a new temperature and humidity processor for the DHT11
         * sensor at the given pin.
         */
        Dht11TemperatureAndHumidity(int pin, const std::string& temperatureTopic,
                const std::string& humidityTopic);
};

class Dht22TemperatureAndHumidity : public DhtTemperatureAndHumidity 
{
    public:
        /**
         * Creates a new temperature and humidity processor for the DHT22
         * sensor at the given pin.
         */
        Dht22TemperatureAndHumidity(int pin, const std::string& temperatureTopic,
                const std::string& humidityTopic);
};

#endif
