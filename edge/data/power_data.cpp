#include "power_data.h"

PowerData::PowerData(time_t timestamp, double avg)
    : BaseSensorData(timestamp, 0, 0, avg, "kWh"), next(nullptr)
{
}

void PowerData::setNext(PowerData *data)
{
    this->next = data;
}

PowerData *PowerData::getNext()
{
    return this->next;
}

