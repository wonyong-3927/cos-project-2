#include "temperature_data.h"

TemperatureData::TemperatureData(time_t timestamp, double min, double max, double avg)
    : BaseSensorData(timestamp, min, max, avg, "celsius"), next(nullptr)
{
}

void TemperatureData::setNext(TemperatureData *data)
{
    this->next = data;
}

TemperatureData *TemperatureData::getNext()
{
    return this->next;
}
