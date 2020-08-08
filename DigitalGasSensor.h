#ifndef DIGITAL_GAS_H
#define DIGITAL_GAS_H

#include <string>
#include "Sensor.h"

/**
 * Processor for the MQ gas sensor family using the digital pin.
 */
class DigitalGasSensor : public Sensor<int, int>
{
    public:
        /**
         * Creates a new gas processor for the sensor at the given digital pin.
         */
        DigitalGasSensor(int digitalPin, const std::string& topic,
                int warningThreshold = 300);

    protected:
        int readValue() const;

        bool isInStableRange(const int& currentValue) const;

        bool isUpdatable(const int& currentValue) const;

        bool isInBadState(const int& currentValue) const;

    private:
        int warningThreshold;

};

#endif
