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
// #include <Arduino_FreeRTOS.h>

// My personally defined headers
#include "capt_portal_page.hpp"
#include "config.h"
#include "web_server.hpp"
#include "capt_portal_connect.hpp"
#include "save_send_module.hpp"







void setup()
{
  // Initializations
  pinMode(RELAY_PIN, INPUT_PULLUP);
  pinMode(CONFIG_TOUCH_0, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RELAY_PIN), set_power_state, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CONFIG_TOUCH_0), CONFIG, RISING);

  Serial.begin(115200);

  Serial.println("Power Monitoring Hub started");

  Initialize_and_connect();
  
  // Check if theres nepa then enter eep sleep mode
  if (digitalRead(RELAY_PIN) == LOW) {
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