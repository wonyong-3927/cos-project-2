#ifndef __HUMIDITY_DATA_H__
#define __HUMIDITY_DATA_H__

#include <random>
#include <ctime>
#include <string>
#include "info.h"
#include "sensor_data.h"

class HumidityData : public BaseSensorData
{
private:
    HumidityData *next;

public:
    HumidityData(time_t timestamp, double min, double max, double avg);
    
    // Next pointer management
    void setNext(HumidityData *data);
    HumidityData *getNext();
};

#endif /* __HUMIDITY_DATA_H__ */
