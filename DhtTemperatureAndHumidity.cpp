#include "DhtTemperatureAndHumidity.h"
#include <Arduino.h>

DhtTemperatureAndHumidity::DhtTemperatureAndHumidity(int pin,
        const std::string& temperatureTopic, const std::string& humidityTopic) : 
    dht(pin, DHT11),
    temperature(pin, dht, temperatureTopic), humidity(pin, dht, humidityTopic) {
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
