#include "DhtHumidity.h"
#include <Arduino.h>

// interval at which to read sensors
#define POLLING_INTERVAL_IN_MS 5000 // 30 secs

// Interval at which to send an MQTT message, even if the temperature or
// humidity value doesn't change.
#define MQTT_PUBLISH_INTERVAL_IN_MS 120000 // 2 minutes

//#define MQTT_TOPIC_HUMIDITY "$topic2"
#define MQTT_TOPIC_HUMIDITY "esp/utility/humidity"

DhtHumidity::DhtHumidity(int pin, DHT& aDht, float threshold) : 
    Sensor(pin, threshold), dht(aDht) {
}

void DhtHumidity::onProcessCycle(AsyncMqttClient& mqttClient,
        unsigned long currentTimeInMs) {
    // sample at the interval
    if (currentTimeInMs - previousTimestampInMs >= POLLING_INTERVAL_IN_MS) {
        previousTimestampInMs = currentTimeInMs;

        float currentHumidity = dht.readHumidity();
        if ( abs(currentHumidity - lastValue) >= threshold
                || (currentTimeInMs - mqttTimestampInMs) 
                        > MQTT_PUBLISH_INTERVAL_IN_MS) {
            lastValue = currentHumidity;

            sentPacketId = mqttClient.publish(
                    MQTT_TOPIC_HUMIDITY, 1, true, String(lastValue).c_str());
            //Serial.printf("Sent to %s (packetId: %i): %.2f\r\n",
            //        MQTT_TOPIC_HUMIDITY, humidityPacketId, humidity);
        }
    }
}
