#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include <random>
#include <ctime>
#include <string>
#include "info.h"
#include "sensor_data.h"

class TemperatureData : public BaseSensorData
{
private:
    TemperatureData *next;

public:
    TemperatureData(time_t timestamp, double min, double max, double avg);
    
    // Next pointer management
    void setNext(TemperatureData *data);
    TemperatureData *getNext();
};

#endif /* __TEMPERATURE_H__ */
