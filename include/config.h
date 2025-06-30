#ifndef config_file
#define config_file

#define relay_pin 14
#define indicator_led 2 
#define config_touch_0 4  


const char* default_ssid = "Power Hub";
const char* default_password = "1234567890";

struct power_details{
  time_t time;
  const char* state;
};

#endif