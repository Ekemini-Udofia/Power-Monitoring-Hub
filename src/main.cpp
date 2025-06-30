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


void flash_nvs() // call this when the nvs flash is full
{
  nvs_flash_erase();
  nvs_flash_init();
}

bool is_power = false; // No nepa ny deault

void log_power_data(power_details& details)
{
  // put detail->time inside preferences?
  // get struct power details and move it to sd card? or just save it in preferences
    // log the power state to a file or database
    // this can be done using LittleFS or SPIFFS for local storage
}

void IRAM_ATTR set_power_state()
{
  is_power = true;
  // add logic to stamp the time and then move that time
  
}





void setup()
{
    pinMode(relay_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(relay_pin), set_power_state, CHANGE);
    Serial.begin(115200);
    Serial.println("Power Monitoring Hub started");
    

    Initialize_and_connect();
    // WiFi.softAP(default_ssid, default_password);

}

void loop()
{
    //int power_state = digitalRead(power_detector_pin);
    
}