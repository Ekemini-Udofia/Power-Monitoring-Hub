#ifndef TIME_SYNC
#define TIME_SYNC

#include "time.h"
#include "capt_portal_connect.hpp"
#include <Arduino.h>

static long time_offset = 0;

void sync_time() {
    if(is_internet_connected())
    {
        configTime(0, 0, "pool.ntp.org");
        while (time(nullptr) < 100000) {
            delay(100);
        }
        time_offset = time(nullptr) - (millis() / 1000);
        Serial.println("Time synced!"); // Debug
    }
    else{
        Serial.println("No internet!"); // Debug
    }
}

inline String get_time() {
    time_t now = (millis() / 1000) + time_offset;
    struct tm * timeinfo = localtime(&now);
    char buffer[16];
    strftime(buffer, sizeof(buffer), "%d/%m/%y", timeinfo);
    return String(buffer);
}

#endif