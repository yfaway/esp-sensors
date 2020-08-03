#ifndef SENSOR_H
#define SENSOR_H

#include <cstdint>
#include <string>
#include <AsyncMqttClient.h>

#include "BaseSensor.h"

/**
 * The template class for various sensor types.
 */
template <typename ValueType, typename ThresholdType>
class Sensor : public BaseSensor
{
    public:
        /**
         * The function type to read the value from the sensor.
         */
        typedef const std::function <ValueType ()> ValueFunction;

        /**
         * The function type to determine if the new value crosses the threshold.
         */
        typedef const std::function <bool (const ValueType, const ValueType,
                const ThresholdType)>
            ThresholdFunction;

    public:
        /**
         * Creates a new gas processor for the sensor at the given analog pin.
         * @param aPin the pin the sensor is connected to
         * @param aMqttTopic the MQTT topic to publish to
         * @param aThreshold the value change threshold at which point an 
         *     MQTT message will be sent.
         * @param aPollingIntervalInMs the interval at which to read from the pin
         * @param aMaxMqttPublishingIntervalInMs the maximum elapsed time to
         *     send a value to the MQTT topic, even if there is no change.
         */
        Sensor(int aPin,
                const std::string& aMqttTopic,
                const ThresholdType& aThreshold = -1,
                const ValueFunction& aValueFunction = [] { return ValueType(); },
                const ThresholdFunction& aThresholdFunction 
                    = Sensor::absDifferenceThresholdFunction(),
                int aPollingIntervalInMs = 5000,
                int aMaxMqttPublishingIntervalInMs = 120000) :
            pin(aPin), mqttTopic(aMqttTopic), threshold(aThreshold),
            valueFunction(aValueFunction),
            thresholdFunction(aThresholdFunction),
            pollingIntervalInMs(aPollingIntervalInMs),
            maxMqttPublishingIntervalInMs(aMaxMqttPublishingIntervalInMs),
            lastValue(), sentPacketId(0), mqttTimestampInMs(0),
            previousTimestampInMs(0) {
        }

        /**
         * Called once by the sketch to set up this sensor.
         */
        virtual void setup() {
        }

        /**
         * A default implementation to read the value, compare it against the
         * threshold, and publish an MQTT message if the value crosses the
         * thershold.
         */
        virtual void onProcessCycle(AsyncMqttClient& mqttClient,
                unsigned long currentTimeInMs) {
            publishValueIf(mqttClient, valueFunction, thresholdFunction);
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
        /**
         * @return the default threshold function that returns true if the
         *     absolute difference between the current value and the last
         *     value is greater or equals to the threshold.
         */
        static ThresholdFunction absDifferenceThresholdFunction() {
            return [](const ValueType& currentValue,
                    const ValueType& lastValue, const ThresholdType& threshold) {
                return abs(currentValue - lastValue) >= threshold;
            };
        }

        /**
         * @return true if the current value is different from the last value.
         *     The threshold value is ignored.
         */
        static ThresholdFunction simpleDifferenceThresholdFunction() {
            return [](const ValueType& currentValue,
                    const ValueType& lastValue, const ThresholdType& threshold) {
                return currentValue != lastValue;
            };
        }

        /**
         * Helper function to retrieve the value, compare against the 
         * threshold and publish an MQTT message if applicable.
         */
        void publishValueIf(AsyncMqttClient& mqttClient,
                const ValueFunction& getValue,
                const ThresholdFunction& crossTheshold) {

            unsigned long currentTimeInMs = millis();
            if (currentTimeInMs - previousTimestampInMs >= pollingIntervalInMs) {
                previousTimestampInMs = currentTimeInMs;

                auto currentValue = getValue();
                if ( crossTheshold(currentValue, lastValue, threshold)
                        || (currentTimeInMs - mqttTimestampInMs) 
                                > maxMqttPublishingIntervalInMs) {
                    lastValue = currentValue;

                    sentPacketId = mqttClient.publish(
                            mqttTopic.c_str(), 1, true, String(lastValue).c_str());

                    Serial.printf("Sent to %s (packetId: %i): %.2f\r\n",
                            mqttTopic.c_str(), sentPacketId, lastValue);
                }
            }
        }

    protected: // input values
        int pin;
        const std::string mqttTopic;
        const ThresholdType threshold;
        const int pollingIntervalInMs;
        const int maxMqttPublishingIntervalInMs;

    protected: // state values
        ValueType lastValue;
        uint16_t sentPacketId;
        unsigned long mqttTimestampInMs; // last mqtt message
        unsigned long previousTimestampInMs;   // the last sensor reading timestamp

    private:
        const ValueFunction valueFunction;
        const ThresholdFunction thresholdFunction;
};

#endif
