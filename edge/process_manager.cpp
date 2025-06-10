#include "process_manager.h"
#include "opcode.h"
#include "byte_op.h"
#include "setting.h"
#include <cstring>
#include <iostream>
#include <ctime>

using namespace std;

// 집계 방식 열거형
enum AggregationType {
    AGG_AVG = 0,
    AGG_CUSTOM = 1,
    AGG_COMFORT = 2  // 불쾌지수 기반 집계
};

// 사용할 집계 방식 선택
const AggregationType aggregationMode = AGG_COMFORT;

ProcessManager::ProcessManager() {
    this->num = 0;
}

void ProcessManager::init() {}

uint8_t *ProcessManager::processData(DataSet *ds, int *dlen) {
    uint8_t *ret, *p;
    ret = (uint8_t *)malloc(BUFLEN);
    memset(ret, 0, BUFLEN);
    *dlen = 0;
    p = ret;

    int num = ds->getNumHouseData();
    HouseData *house;
    TemperatureData *tdata = ds->getTemperatureData();
    HumidityData *hdata = ds->getHumidityData();
    PowerData *pdata;
    time_t ts = ds->getTimestamp();
    struct tm *tm = localtime(&ts);

    // 평균값 기반 집계
    if (aggregationMode == AGG_AVG) {
        int avg_temp = (int)tdata->getValue();
        int avg_humid = (int)hdata->getValue();

        int total_power = 0;
        for (int i = 0; i < num; i++) {
            house = ds->getHouseData(i);
            pdata = house->getPowerData();
            total_power += (int)pdata->getValue();
        }
        int avg_power = total_power / max(1, num);
        int month = tm->tm_mon + 1;

        VAR_TO_MEM_1BYTE_BIG_ENDIAN(avg_temp, p); *dlen += 1;
        VAR_TO_MEM_1BYTE_BIG_ENDIAN(avg_humid, p); *dlen += 1;
        VAR_TO_MEM_2BYTES_BIG_ENDIAN(avg_power, p); *dlen += 2;
        VAR_TO_MEM_1BYTE_BIG_ENDIAN(month, p); *dlen += 1;
    }

    // 최대 전력, 습도 버킷, 요일
    else if (aggregationMode == AGG_CUSTOM) {
        int max_power = 0, tmp;
        for (int i = 0; i < num; i++) {
            house = ds->getHouseData(i);
            pdata = house->getPowerData();
            tmp = (int)pdata->getValue();
            if (tmp > max_power) max_power = tmp;
        }

        int avg_humid = (int)hdata->getValue();
        int humid_bucket = 0;
        if (avg_humid < 30) humid_bucket = 0;
        else if (avg_humid < 60) humid_bucket = 1;
        else humid_bucket = 2;

        int weekday = tm->tm_wday;

        VAR_TO_MEM_2BYTES_BIG_ENDIAN(max_power, p); *dlen += 2;
        VAR_TO_MEM_1BYTE_BIG_ENDIAN(humid_bucket, p); *dlen += 1;
        VAR_TO_MEM_1BYTE_BIG_ENDIAN(weekday, p); *dlen += 1;
    }

    // 불쾌지수 기반 집계
    else if (aggregationMode == AGG_COMFORT) {
        int avg_temp = (int)tdata->getValue();
        int avg_humid = (int)hdata->getValue();

        // 불쾌지수 계산
        double di = 0.81 * avg_temp + 0.01 * avg_humid * (0.99 * avg_temp - 14.3) + 46.3;
        int discomfort_index = (int)(di + 0.5);  // 반올림

        int weekday = tm->tm_wday;

        VAR_TO_MEM_1BYTE_BIG_ENDIAN(avg_temp, p); *dlen += 1;
        VAR_TO_MEM_1BYTE_BIG_ENDIAN(avg_humid, p); *dlen += 1;
        VAR_TO_MEM_1BYTE_BIG_ENDIAN(discomfort_index, p); *dlen += 1;
        VAR_TO_MEM_1BYTE_BIG_ENDIAN(weekday, p); *dlen += 1;
    }

    return ret;
}
