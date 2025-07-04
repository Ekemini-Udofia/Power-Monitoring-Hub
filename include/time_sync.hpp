#ifndef TIME_SYNC
#define TIME_SYNC

#include "time.h"
#include "capt_portal_connect.hpp"

void sync_time() {
    if(is_internet_connected())
    {
        configTime(0, 0, "pool.ntp.org");
        while (time(nullptr) < 100000) {
            delay(100);
        }
        Serial.println("Time synced!"); // Debug
    }
    else{
        Serial.println("No internet!"); // Debug
    }
}

#endif