#include "process_manager.h"
#include "opcode.h"
#include "byte_op.h"
#include "setting.h"
#include <cstring>
#include <iostream>
#include <ctime>

using namespace std;

// 선택할 수 있는 집계 방식
enum AggregationType {
    AGG_AVG = 0,
    AGG_CUSTOM = 1,
    AGG_STATS = 2  // 예: 평균 + 표준편차 등 향후 확장용
};

// 현재 사용할 집계 방식 지정
const AggregationType aggregationMode = AGG_AVG;

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

    if (aggregationMode == AGG_AVG) {
        // 평균 온도와 습도는 getValue()로 대체
        int avg_temp = (int)tdata->getValue();
        int avg_humid = (int)hdata->getValue();

        // 평균 전력 계산
        int total_power = 0;
        for (int i = 0; i < num; i++) {
            house = ds->getHouseData(i);
            pdata = house->getPowerData();
            total_power += (int)pdata->getValue();
        }
        int avg_power = total_power / max(1, num);  // 0으로 나눔 방지

        int month = tm->tm_mon + 1;

        VAR_TO_MEM_1BYTE_BIG_ENDIAN(avg_temp, p); *dlen += 1;
        VAR_TO_MEM_1BYTE_BIG_ENDIAN(avg_humid, p); *dlen += 1;
        VAR_TO_MEM_2BYTES_BIG_ENDIAN(avg_power, p); *dlen += 2;
        VAR_TO_MEM_1BYTE_BIG_ENDIAN(month, p); *dlen += 1;
    }

    else if (aggregationMode == AGG_CUSTOM) {
        // 최대 전력, 습도 버킷, 요일 정보
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

    // 향후 AGG_STATS 등 다른 집계 방식을 여기에 추가 가능

    return ret;
}
