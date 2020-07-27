#include "TemperatureAndHumidity.h"
#include <Arduino.h>

// interval at which to read sensors
#define POLLING_INTERVAL_IN_MS 5000 // 30 secs

// Interval at which to send an MQTT message, even if the temperature or
// humidity value doesn't change.
#define MQTT_PUBLISH_INTERVAL_IN_MS 120000 // 2 minutes

//#define MQTT_TOPIC_TEMPERATURE "$topic1"
//#define MQTT_TOPIC_HUMIDITY "$topic2"

#define MQTT_TOPIC_TEMPERATURE "esp/utility/temperature"
#define MQTT_TOPIC_HUMIDITY "esp/utility/humidity"

const float TemperatureAndHumidity::DIFFERENCE_THRESHOLD = 0.1;

TemperatureAndHumidity::TemperatureAndHumidity(int pin) : 
    dht(pin, DHT11),
    temperature(0), humidity(0),
    temperaturePacketId(0), humidityPacketId(0),
    mqttTemperatureTimestampInMs(0), mqttHumidityTimestampInMs(0),
    previousTimestampInMs(0) {
}

void TemperatureAndHumidity::setup() {
    dht.begin();
}

void TemperatureAndHumidity::onProcessCycle(AsyncMqttClient& mqttClient,
        unsigned long currentTimeInMs) {
    // sample at the interval
    if (currentTimeInMs - previousTimestampInMs >= POLLING_INTERVAL_IN_MS) {
        previousTimestampInMs = currentTimeInMs;

        float currentTemperature = dht.readTemperature();
        if ( abs(currentTemperature - temperature) >= DIFFERENCE_THRESHOLD
                || (currentTimeInMs - mqttTemperatureTimestampInMs)
                        > MQTT_PUBLISH_INTERVAL_IN_MS) {
            temperature = currentTemperature;

            temperaturePacketId = mqttClient.publish(
                    MQTT_TOPIC_TEMPERATURE, 1, true, String(temperature).c_str());

            //Serial.printf("Sent to %s (packetId: %i): %.2f\r\n",
            //        MQTT_TOPIC_TEMPERATURE, temperaturePacketId, temperature);
        }

        float currentHumidity = dht.readHumidity();
        if ( abs(currentHumidity - humidity) >= DIFFERENCE_THRESHOLD
                || (currentTimeInMs - mqttHumidityTimestampInMs) 
                        > MQTT_PUBLISH_INTERVAL_IN_MS) {
            humidity = currentHumidity;

            humidityPacketId = mqttClient.publish(
                    MQTT_TOPIC_HUMIDITY, 1, true, String(humidity).c_str());                            

            //Serial.printf("Sent to %s (packetId: %i): %.2f\r\n",
            //        MQTT_TOPIC_HUMIDITY, humidityPacketId, humidity);
        }
    }
}

void TemperatureAndHumidity::onMqttPacketAcknowledged(uint16_t packetId) {
    if ( packetId == temperaturePacketId ) {
        mqttTemperatureTimestampInMs = millis();
    }
    else if ( packetId == humidityPacketId ) {
        mqttHumidityTimestampInMs = millis();
    }
}

