#ifndef time_sync
#define time_sync

#include "time.h"
#include "capt_portal_connect.hpp"


bool is_internet = is_internet_connected();

void sync_time() {
    if(is_internet)
    {
        configTime(0, 0, "pool.ntp.org");
        while (time(nullptr) < 100000) {
            delay(100);
        }
        Serial.println("Time synced!");
    }
}

#endif