#ifndef ANALOG_GAS_H
#define ANALOG_GAS_H

#include <string>
#include "Sensor.h"

/**
 * Processor for the MQ gas sensor family using the analog pin.
 */
class AnalogGasSensor : public Sensor<int, int>
{
    public:
        /**
         * Creates a new gas processor for the sensor at the given analog pin.
         * @param threshold the difference between the current value and the
         *     last value, at which point an MQTT message will be sent out.
         */
        AnalogGasSensor(int analogPin, const std::string& topic, int threshold = 3);
};

#endif
