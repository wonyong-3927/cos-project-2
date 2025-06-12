#include <iostream>
#include <ctime>
using namespace std;

int main() {
    // 몇 개의 타임스탬프에 대해 요일 확인
    time_t timestamps[] = {
        1609459200,  // 2021-01-01 
        1609545600,  // 2021-01-02
        1609632000,  // 2021-01-03
        1609718400,  // 2021-01-04
        1609804800   // 2021-01-05
    };
    
    for (int i = 0; i < 5; i++) {
        struct tm *tm = localtime(&timestamps[i]);
        if (tm) {
            cout << "Timestamp: " << timestamps[i] << endl;
            cout << "  Date: " << (tm->tm_year + 1900) << "-" << (tm->tm_mon + 1) << "-" << tm->tm_mday << endl;
            cout << "  Weekday (tm_wday): " << tm->tm_wday << endl;
            cout << "  Day name: ";
            switch(tm->tm_wday) {
                case 0: cout << "Sunday"; break;
                case 1: cout << "Monday"; break;
                case 2: cout << "Tuesday"; break;
                case 3: cout << "Wednesday"; break;
                case 4: cout << "Thursday"; break;
                case 5: cout << "Friday"; break;
                case 6: cout << "Saturday"; break;
            }
            cout << endl << endl;
        }
    }
    
    return 0;
}
