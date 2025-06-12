#ifndef __SENSOR_DATA_H__
#define __SENSOR_DATA_H__

#include <ctime>
#include <string>
#include "info.h"

// Base class for sensor data without templates
class BaseSensorData
{
protected:
    time_t timestamp;
    double avg;
    double min;
    double max;
    string unit;

public:
    BaseSensorData(time_t timestamp, double min, double max, double avg, const string& unit_value)
        : timestamp(timestamp), avg(avg), min(min), max(max), unit(unit_value) {}

    virtual ~BaseSensorData() = default;

    // Value management
    void setValue(double value) { this->avg = value; }
    double getValue() const { return this->avg; }

    // Min/Max management
    void setMin(double min_value) { this->min = min_value; }
    double getMin() const { return this->min; }

    void setMax(double max_value) { this->max = max_value; }
    double getMax() const { return this->max; }

    // Timestamp management
    void setTimestamp(time_t timestamp_value) { this->timestamp = timestamp_value; }
    time_t getTimestamp() const { return this->timestamp; }

    // Unit management
    string getUnit() const { return this->unit; }
    void setUnit(const string& unit_value) { this->unit = unit_value; }
};

#endif /* __SENSOR_DATA_H__ */
