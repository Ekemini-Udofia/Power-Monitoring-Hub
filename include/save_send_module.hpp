#ifndef THING_SPEAK_MODULE
#define THING_SPEAK_MODULE

#include "web_server.hpp"
#include "time_sync.hpp"
#include "ThingSpeak.h"
#include "secrets.hpp" // For thingspeak API Key

void save_log(power_details& details)
{
  // put detail->time inside preferences? - Later

  String time = details.time, state = details.state;
  WiFiClient tsClient;
  ThingSpeak.begin(tsClient);
  ThingSpeak.setField(1, time);
  ThingSpeak.setField(2, state);

  int statusCode = ThingSpeak.writeFields(myChannelNumber, SECRET_READ_API_KEY);
  String status = (statusCode == 200) ? "Channel Update Successful!" : "Couldn't Update Channel!";
  Serial.println(status);
}

void IRAM_ATTR setPowerState()
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