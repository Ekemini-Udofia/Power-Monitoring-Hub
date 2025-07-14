#ifndef THING_SPEAK_MODULE
#define THING_SPEAK_MODULE

#include "web_server.hpp"
#include "time_sync.hpp"
#include "ThingSpeak.h"
#include "secrets.hpp" // For thingspeak API Key

void save_log(power_details& details)
{
  // put detail->time inside preferences?
  // get struct power details and move it to sd card? or just save it in preferences
    // log the power state to a file or database
    // this can be done using LittleFS or SPIFFS for local storage



}

void IRAM_ATTR set_power_state()
{
  power_details details;
  is_power = !is_power; // change it to its Opposite state
  // add logic to stamp the time and then move that time
  if(is_power){
    details.state = "NEPA On";
    details.time = get_time();
  }else{
    details.state = "NEPA Off";
    details.time = get_time();
  }
  save_log(details);  
}

#endif