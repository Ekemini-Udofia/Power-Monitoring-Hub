#ifndef CONFIG_FILE
#define CONFIG_FILE

#define RELAY_PIN 14
#define RELAY_PIN_2 26
#define INDICATOR_LED 2 
#define CONFIG_TOUCH_0 4  


const char* default_ssid = "Power Hub";
const char* default_password = "1234567890";

struct power_details{
  time_t time;
  const char* state;
};

#endif