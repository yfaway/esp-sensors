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
        AnalogGasSensor(int analogPin, const std::string& topic, 
                int warningThreshold = 300, int updateThreshold = 3);

    protected:
        int readValue() const;

        bool isInStableRange(const int& currentValue) const;

        /**
         * Returns true if the value indicates that the sensor has detected
         * the associated gas above the threshold.
         * 
         * To avoid spontaneous jump in the gas value (obseved with the CO2
         * sensor), this method won't return false right after the value 
         * passes the threshold. Instead it would wait for couple of readings
         * above the threshold before returning true.
         */
        bool isInBadState(const int& currentValue);

    private:
        int warningThreshold;
        
        // track how many time the value has continuously pass the threshold.
        int badStateCount;
};

#endif
