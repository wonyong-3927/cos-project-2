#include "humidity_data.h"

HumidityData::HumidityData(time_t timestamp, double min, double max, double avg)
    : BaseSensorData(timestamp, min, max, avg, "%"), next(nullptr)
{
}

void HumidityData::setNext(HumidityData *data)
{
    this->next = data;
}

HumidityData *HumidityData::getNext()
{
    return this->next;
}
