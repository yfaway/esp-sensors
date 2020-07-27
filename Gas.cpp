#include "Gas.h"
#include <Arduino.h>

// interval at which to read sensors
#define POLLING_INTERVAL_IN_MS 2000 // 2 secs

// Interval at which to send an MQTT message, even if the value doesn't change
#define MQTT_PUBLISH_INTERVAL_IN_MS 120000 // 2 minutes

#define MQTT_TOPIC "esp/utility/gas"

const int Gas::DIFFERENCE_THRESHOLD = 3;

Gas::Gas(int analogPin) : 
    pin(analogPin), lastValue(0), sentPacketId(0), mqttTimestampInMs(0),
    previousTimestampInMs(0) {
}

void Gas::setup() {
}

void Gas::onProcessCycle(AsyncMqttClient& mqttClient,
        unsigned long currentTimeInMs) {
    // sample at the interval
    if (currentTimeInMs - previousTimestampInMs >= POLLING_INTERVAL_IN_MS) {
        previousTimestampInMs = currentTimeInMs;

        int currentValue = analogRead(pin);

        if ( abs(currentValue - lastValue) >= DIFFERENCE_THRESHOLD
                || (currentTimeInMs - mqttTimestampInMs)
                        > MQTT_PUBLISH_INTERVAL_IN_MS) {
            lastValue = currentValue;

            sentPacketId = mqttClient.publish(
                    MQTT_TOPIC, 1, true, String(lastValue).c_str());

            Serial.printf("Sent to %s (packetId: %i): %.2f\r\n",
                    MQTT_TOPIC, sentPacketId, lastValue);
        }
    }
}

void Gas::onMqttPacketAcknowledged(uint16_t packetId) {
    if ( packetId == sentPacketId ) {
        mqttTimestampInMs = millis();
    }
}
