#ifndef time_sync
#define time_sync

#include "time.h"
#include "capt_portal_connect.hpp"


bool is_internet = is_internet_connected();
long internet_time()
{
    if(is_internet_connected())
    {
        configTime(0, 0, "pool.ntp.org");
        struct tm timeinfo;
        return getLocalTime(&timeinfo);
    } else{
        return NULL;
    }
}

#endif