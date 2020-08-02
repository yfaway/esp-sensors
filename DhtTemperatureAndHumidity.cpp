#include "DhtTemperatureAndHumidity.h"
#include <Arduino.h>

DhtTemperatureAndHumidity::DhtTemperatureAndHumidity(int pin) : 
    dht(pin, DHT11),
    temperature(pin, dht), humidity(pin, dht) {
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
