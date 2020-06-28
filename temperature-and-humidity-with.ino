/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-mqtt-publish-dht11-dht22-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

/**
 * Further modifications:
 *  - Add throttling so that the MQTT messages are only sent if the temperature 
 *    or humidity change is larger than 0.1. Otherwise, send the message anyway
 *    after a specific period (to take care of the case when mqtt client comes
 *    online later..
 *  - Various structure/name changes (still quite similar to the old code).
 */

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>

#define WIFI_SSID "$wifiSsid"
#define WIFI_PASSWORD "$wifiPassword"

#define MQTT_HOST IPAddress(192, 168, 0, 204)
#define MQTT_PORT 1883

#define MQTT_TOPIC_TEMPERATURE "$temperatureTopic"
#define MQTT_TOPIC_HUMIDITY "$humidityTopic"

#define DHT_PIN 14 // digital pin connected to the DHT sensor

#define DHT_TYPE DHT11   // or DHT21 or DHT22

#define POLLING_INTERVAL_IN_MS 10000 // interval at which to read sensors

// Interval at which to send an MQTT message, even if the temperature or
// humidity value doesn't change.
#define MQTT_PUBLISH_INTERVAL_IN_MS 60000

DHT dht(DHT_PIN, DHT_TYPE);

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

float temperature = 0;
float humidity = 0;

uint16_t temperaturePacketId = 0; // mqtt packet id
uint16_t humidityPacketId = 0;    // mqtt packet id

unsigned long mqttTemperatureTimestampInMs = 0; // last mqtt temp message
unsigned long mqttHumidityTimestampInMs = 0; // last mqtt humidity message

unsigned long previousTimestampInMs = 0;   // the last sensor reading timestamp

void connectToWifi() {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
    Serial.println("Connected to Wi-Fi.");
    connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
    Serial.println("Disconnected from Wi-Fi.");
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
    Serial.println("Connecting to MQTT...");
    mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
    Serial.println("Connected to MQTT");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.println("Disconnected from MQTT.");

    if (WiFi.isConnected()) {
        mqttReconnectTimer.once(2, connectToMqtt);
    }
}

void onMqttPublish(uint16_t packetId) {
    if ( packetId == temperaturePacketId ) {
        mqttTemperatureTimestampInMs = millis();
    }
    else if ( packetId == humidityPacketId ) {
        mqttHumidityTimestampInMs = millis();
    }

    Serial.printf("MQTT server acknowledged packet %i\r\n", packetId);
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    dht.begin();

    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);

    connectToWifi();
}

void loop() {
    unsigned long currentTimeInMs = millis();

    // sample at the interval
    if (currentTimeInMs - previousTimestampInMs >= POLLING_INTERVAL_IN_MS) {
        previousTimestampInMs = currentTimeInMs;

        float currentTemperature = dht.readTemperature();
        if ( abs(currentTemperature - temperature) >= 0.1
                || (currentTimeInMs - mqttTemperatureTimestampInMs)
                        > MQTT_PUBLISH_INTERVAL_IN_MS) {
            temperature = currentTemperature;

            temperaturePacketId = mqttClient.publish(
                    MQTT_TOPIC_TEMPERATURE, 1, true, String(temperature).c_str());

            Serial.printf("Sent to %s (packetId: %i): %.2f\r\n",
                    MQTT_TOPIC_TEMPERATURE, temperaturePacketId, temperature);
        }

        float currentHumidity = dht.readHumidity();
        if ( abs(currentHumidity - humidity) >= 0.1 
                || (currentTimeInMs - mqttHumidityTimestampInMs) 
                        > MQTT_PUBLISH_INTERVAL_IN_MS) {
            humidity = currentHumidity;

            humidityPacketId = mqttClient.publish(
                    MQTT_TOPIC_HUMIDITY, 1, true, String(humidity).c_str());                            

            Serial.printf("Sent to %s (packetId: %i): %.2f\r\n",
                    MQTT_TOPIC_HUMIDITY, humidityPacketId, humidity);
        }
    }
}
