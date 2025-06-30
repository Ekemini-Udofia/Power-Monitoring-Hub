/*Power Monitoring Hub project

    Description: 
        This is the code for the power monitoring system. 
        The device/system uses a relay to know when power(NEPA) in the house is active.
        
        It (should have) a feature to display that data of power downtime and uptime on 
        a webpage hosted both locally and on the web, to enable one see it from anywhere
        in the world

    How it works:
        Detect Power Supply
        Use a GPIO pin to detect mains power presence (via optocoupler or relay module).

        Debounce & Timestamp
        On state change (light on/off), debounce and log with millis() or NTP-based timestamp.

        Store Events
        Save logs in RAM or LittleFS (e.g., JSON array of timestamps and states).

        Serve Web Page
        Set up ESP32 web server to serve a live dashboard (HTML/JS) showing logs/status.

        Live Updates (Optional)
        Use WebSockets or AJAX polling to push real-time status to the browser.

        Predictive API Hook (Optional)
        Periodically send logs to Gemini API (or similar) and display predictions on the page.
*/


// Include libraries
#include <Arduino.h>
#include <nvs_flash.h>
// #include <Arduino_FreeRTOS.h>

// My personally defined headers
#include "capt_portal_page.hpp"
#include "config.h"
#include "web_server.hpp"
#include "capt_portal_connect.hpp"
#include "time_sync.hpp"


void flash_nvs() // call this when the nvs flash is full
{
  nvs_flash_erase();
  nvs_flash_init();
}

bool is_power = false; // No nepa ny deault

void save_log(power_details& details)
{
  // put detail->time inside preferences?
  // get struct power details and move it to sd card? or just save it in preferences
    // log the power state to a file or database
    // this can be done using LittleFS or SPIFFS for local storage
}

time_t log_time_data() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    Serial.printf("State changed at: %s\n", buffer);
    time_t power_time = buffer[64];
    return power_time;

}

void IRAM_ATTR set_power_state()
{
  power_details details;
  is_power = !is_power; // change it to its Opposite state
  // add logic to stamp the time and then move that time
  if(is_power){
    details.state = "NEPA On";
    details.time = log_time_data();
  }else{
    details.state = "NEPA Off";
    details.time = log_time_data();
  }
  save_log(details);  
}





void setup()
{
  // Initializations
  pinMode(relay_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(relay_pin), set_power_state, CHANGE);
  Serial.begin(115200);

  Serial.println("Power Monitoring Hub started");

  Initialize_and_connect();
  
  // Check if theres nepa then enter eep sleep mode
  if (digitalRead(relay_pin) == LOW) {
    Serial.println("No power. Sleeping...");
    delay(2000);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_1, 1);
    esp_deep_sleep_start();
  }
    // WiFi.softAP(default_ssid, default_password);

}

void loop()
{
    
}