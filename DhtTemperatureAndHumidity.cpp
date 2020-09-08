#include "DhtTemperatureAndHumidity.h"
#include <Arduino.h>

DhtTemperatureAndHumidity::DhtTemperatureAndHumidity(int dhtType, int pin,
        const std::string& temperatureTopic, const std::string& humidityTopic) : 
    dht(pin, dhtType),
    temperature(pin, dht, temperatureTopic),
    humidity(pin, dht, humidityTopic) {
}

void DhtTemperatureAndHumidity::setup() {
    dht.begin();
}

void DhtTemperatureAndHumidity::onProcessCycle(AsyncMqttClient& mqttClient,
        unsigned long currentTimeInMs) {
    temperature.onProcessCycle(mqttClient, currentTimeInMs);
    humidity.onProcessCycle(mqttClient, currentTimeInMs);
}

void DhtTemperatureAndHumidity::onMqttPacketAcknowledged(uint16_t packetId) {
    temperature.onMqttPacketAcknowledged(packetId);
    humidity.onMqttPacketAcknowledged(packetId);
}


Dht11TemperatureAndHumidity::Dht11TemperatureAndHumidity(
        int pin, const std::string& temperatureTopic,
        const std::string& humidityTopic) :
    DhtTemperatureAndHumidity(DHT11, pin, temperatureTopic, humidityTopic)
{
}

Dht22TemperatureAndHumidity::Dht22TemperatureAndHumidity(
        int pin, const std::string& temperatureTopic,
        const std::string& humidityTopic) :
    DhtTemperatureAndHumidity(DHT22, pin, temperatureTopic, humidityTopic)
{
}
