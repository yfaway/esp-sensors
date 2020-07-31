#include "DhtTemperature.h"
#include <Arduino.h>

// interval at which to read sensors
#define POLLING_INTERVAL_IN_MS 5000 // 30 secs

// Interval at which to send an MQTT message, even if the temperature or
// humidity value doesn't change.
#define MQTT_PUBLISH_INTERVAL_IN_MS 120000 // 2 minutes

//#define MQTT_TOPIC_TEMPERATURE "$topic1"
#define MQTT_TOPIC_TEMPERATURE "esp/utility/temperature"

DhtTemperature::DhtTemperature(int pin, float threshold) : 
    Sensor(pin, threshold), dht(pin, DHT11) {
}

void DhtTemperature::setup() {
    dht.begin();
}

void DhtTemperature::onProcessCycle(AsyncMqttClient& mqttClient,
        unsigned long currentTimeInMs) {
    // sample at the interval
    if (currentTimeInMs - previousTimestampInMs >= POLLING_INTERVAL_IN_MS) {
        previousTimestampInMs = currentTimeInMs;

        float currentValue = dht.readTemperature();
        if ( abs(currentValue - lastValue) >= threshold
                || (currentTimeInMs - mqttTimestampInMs) 
                        > MQTT_PUBLISH_INTERVAL_IN_MS) {
            lastValue = currentValue;

            sentPacketId = mqttClient.publish(
                    MQTT_TOPIC_TEMPERATURE, 1, true, String(lastValue).c_str());
        }
    }
}
