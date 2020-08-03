/*
 * The main integrator for various sensors.
 *
 * Initial sketch structure idea is from https://RandomNerdTutorials.com/esp8266-nodemcu-mqtt-publish-dht11-dht22-arduino/
 * However, the code has been heavily modified to offload the heavy work for
 * each sensor into a separate module. This file acts as the main integrator,
 * and takes care of just the WiFi and MQTT connectivity.
 * 
 * Each sensor has additional throttling so that the MQTT messages are only
 * sent if the value change is above a threshold. Otherwise, send the message
 * anyway after a specific period (to take care of the case when mqtt client
 * comes online later.
 *
 * @see EspHome for a much easier solution. I am doing it from the beginning
 *      mainly for fun, and to get back to C/C++ programming again. I also 
        want further reduction in MQTT traffic.
 */

#include <vector>

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>

#include "AnalogGasSensor.h"
#include "DigitalGasSensor.h"
#include "DhtTemperatureAndHumidity.h"
#include "BaseSensor.h"

#define WIFI_SSID "$wifiSsid"
#define WIFI_PASSWORD "$wifiPassword"

#define MQTT_HOST IPAddress(192, 168, 0, 204)
#define MQTT_PORT 1883

#define DHT_PIN 14 // digital pin connected to the DHT sensor

//#define DHT_TYPE DHT11   // or DHT21 or DHT22

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

const std::vector<BaseSensor*> sensors {
    new DhtTemperatureAndHumidity(14, "esp/utility/temperature",
            "esp/utility/humidity"),
    new AnalogGasSensor(A0, "esp/utility/naturalGas")
};

void connectToMqtt();

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
    for (auto* sensor : sensors) {
        sensor->onMqttPacketAcknowledged(packetId);
    }

    Serial.printf("MQTT server acknowledged packet %i\r\n", packetId);
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    for (auto* sensor : sensors) {
        sensor->setup();
    }

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

    for (auto* sensor : sensors) {
        sensor->onProcessCycle(mqttClient, currentTimeInMs);
    }
}
