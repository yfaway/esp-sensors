#include "DigitalGasSensor.h"
#include <Arduino.h>

// interval at which to read sensors
#define POLLING_INTERVAL_IN_MS 2000 // 2 secs

// Interval at which to send an MQTT message, even if the value doesn't change
#define MQTT_PUBLISH_INTERVAL_IN_MS 120000 // 2 minutes

#define MQTT_TOPIC "esp/utility/gas"

DigitalGasSensor::DigitalGasSensor(int pin) : Sensor(pin) {
}

void DigitalGasSensor::onProcessCycle(AsyncMqttClient& mqttClient,
        unsigned long currentTimeInMs) {
    // sample at the interval
    if (currentTimeInMs - previousTimestampInMs >= POLLING_INTERVAL_IN_MS) {
        previousTimestampInMs = currentTimeInMs;

        int currentValue = digitalRead(pin);

        if ( currentValue != lastValue ) {
            lastValue = currentValue;

            sentPacketId = mqttClient.publish(
                    MQTT_TOPIC, 1, true, String(lastValue).c_str());

            Serial.printf("Sent to %s (packetId: %i): %.2f\r\n",
                    MQTT_TOPIC, sentPacketId, lastValue);
        }
    }
}
