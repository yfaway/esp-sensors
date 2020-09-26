#ifndef SENSOR_H
#define SENSOR_H

#include <cstdint>
#include <string>
#include <utility>
#include <AsyncMqttClient.h>

#include "BaseSensor.h"

/**
 * The template class for various sensor types.
 * Provide support for default support for reading from the sensor and sending
 * update to the MQTT topic.
 * There are 3 topics for each sensor:
 *   1. $baseTopic/value: the raw value.
 *   2. $baseTopic/stable: an 0 or 1 value indicating if the sensor is in
 *      a stable state.
 *   3. $baseTopic/state: an 0 or 1 value indicating if the sensor is in
 *      a bad state; i.e. it detects a bad value (e.g. smoke or natural gas).
 */
template <typename ValueType, typename ThresholdType>
class Sensor : public BaseSensor
{
    public:
        /**
         * Creates a new gas processor for the sensor at the given analog pin.
         * @param aPin the pin the sensor is connected to
         * @param aMqttBaseTopic the base MQTT topic to publish to. The value
         *     read from the sensor will be updated to "$mqttBaseTopic/value".
         *     Some sensors also require sometimes until the value stabelize.
         *     For these sensors, an additional topic "$mqttBaseTopic/isStable"
         *     is also used.
         * @param aUpdateThreshold the value change threshold at which point an 
         *     MQTT message will be sent.
         * @param aPollingIntervalInMs the interval at which to read from the pin
         * @param aMaxMqttPublishingIntervalInMs the maximum elapsed time to
         *     send a value to the MQTT topic, even if there is no change.
         */
        Sensor(int aPin,
                const std::string& aMqttBaseTopic,
                const ThresholdType& aUpdateThreshold = -1,
                const ValueType& aStabilityThreshold = ValueType(),
                int aPollingIntervalInMs = 5000,
                int aMaxMqttPublishingIntervalInMs = 120000) :
            pin(aPin), mqttBaseTopic(aMqttBaseTopic),
            updateThreshold(aUpdateThreshold),
            stabilityThreshold(aStabilityThreshold),
            pollingIntervalInMs(aPollingIntervalInMs),
            maxMqttPublishingIntervalInMs(aMaxMqttPublishingIntervalInMs),
            lastValue(), sentPacketId(0), mqttTimestampInMs(0),
            previousTimestampInMs(0),
            stable(false) {
        }

        /**
         * Called once by the sketch to set up this sensor.
         */
        virtual void setup() {
        }

        /**
         * A default implementation to read the value, compare it against the
         * updateThreshold, and publish an MQTT message if the value crosses the
         * threshold.
         */
        virtual void onProcessCycle(AsyncMqttClient& mqttClient,
                unsigned long currentTimeInMs) {

            auto result = readAndPublishValue(mqttClient);
            if ( result.first ) {
                // send stability value
                if ( ! stable ) {
                    stable = isInStableRange(result.second);
                }
                std::string stableTopic = mqttBaseTopic + "/stable";
                sentPacketId = sendValue(mqttClient, stableTopic, 
                        stable ? 1 : 0);

                // send state value
                std::string stateTopic = mqttBaseTopic + "/state";
                sentPacketId = sendValue(mqttClient, stateTopic, 
                        (stable && isInBadState(result.second)) ? 1 : 0);
            }
        }

        /**
         * Called by the main sketch when it receives the MQTT ack.
         */
        virtual void onMqttPacketAcknowledged(uint16_t packetId) {
            if ( packetId == sentPacketId ) {
                mqttTimestampInMs = millis();
            }
        }

    protected: 
        virtual ValueType readValue() const = 0;

        /**
         * Return true if that the sensor value has reached the stable range;
         * i.e. it has fully initialized.
         * This method is continuously called each time a value is available
         * until the value has settled in a stable range, at which point
         * the value '1' will be sent to the the 'stable' MQTT channel.
         */
        virtual bool isInStableRange(const ValueType& currentValue) const {
            return true;
        }

        /**
         * Return a boolean value to indicate if the value is in a bad 
         * state (e.g. the sensor detects smoke or natural gas).
         * If the returned value is true, the value '1' will be sent to the
         * 'state' MQTT channel.
         */
        virtual bool isInBadState(const ValueType& currentValue) {
            return false;
        }

        /**
         * Returns true if the current value is signficantly different (based
         * on the threshold) compared to the last value.
         */
        virtual bool isUpdatable(const ValueType& currentValue) const {
            return abs(currentValue - lastValue) > updateThreshold;
        }

    protected:
        /**
         * Helper function to retrieve the value, compare against the 
         * update threshold and publish an MQTT message if applicable.
         * @return a pair of boolean value indicating whether the value is
         *     published or not, and the actual value.
         */
        std::pair<bool, ValueType> readAndPublishValue(AsyncMqttClient& mqttClient) {

            unsigned long currentTimeInMs = millis();
            if (currentTimeInMs - previousTimestampInMs >= pollingIntervalInMs) {
                previousTimestampInMs = currentTimeInMs;
                auto currentValue = readValue();

                //Serial.printf("Value for %s: %s\n", mqttBaseTopic.c_str(), String(currentValue).c_str());
                if ( isUpdatable(currentValue)
                        || isBeyondPublishingPeriod() ) {

                    lastValue = currentValue;

                    std::string valueTopic = mqttBaseTopic + "/value";
                    sentPacketId = sendValue(mqttClient, valueTopic, lastValue);

                    return std::make_pair(true, currentValue);
                }
            }

            return std::make_pair(false, ValueType());
        }

        /**
         * Returns true if the current value must be sent even if it is not
         * significantly different compared to the last value.
         */
        bool isBeyondPublishingPeriod() const {
            unsigned long currentTimeInMs = millis();
            return (currentTimeInMs - mqttTimestampInMs) 
                > maxMqttPublishingIntervalInMs;
        }

    private:
        /**
         * Helper method to send an MQTT message and to log the value.
         */
        uint16_t sendValue(AsyncMqttClient& mqttClient,
                const std::string& topic, const ValueType& value) {
            uint16_t packetId = mqttClient.publish(
                    topic.c_str(), 1, true, String(value).c_str());

            Serial.printf("Sent to %s (packetId: %i): %s\n",
                    topic.c_str(), packetId, String(value).c_str());

            return packetId;
        }

    protected: // input values
        int pin;
        const std::string mqttBaseTopic;
        const ThresholdType updateThreshold;
        const ValueType stabilityThreshold;
        const int pollingIntervalInMs;
        const int maxMqttPublishingIntervalInMs;

    protected: // state values
        ValueType lastValue;
        uint16_t sentPacketId;
        unsigned long mqttTimestampInMs; // last mqtt message
        unsigned long previousTimestampInMs;   // the last sensor reading timestamp
        bool stable;
};

#endif
