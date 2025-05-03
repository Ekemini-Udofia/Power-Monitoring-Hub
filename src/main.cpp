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



#include <Arduino.h>
#include <WiFi.h>

#define power_detector_pin 1// change this as needed
#define indicator_led 2 //for the led bulb. Change this when an external indicator bulb has been put 


const char* ssid = "Power Hub";
const char* password = "1234567890";



void create_network()
{
    WiFi.softAP(ssid, password);
}


/*
void create_web_server()
{
    // create a web server to serve the webpage
    // this can be done using the ESPAsyncWebServer library or the WebServer library
}

void create_captive_portal()
{
    // create captive portal and direct user to captive portal page.
}

void log_power_data()
{
    // log the power state to a file or database
    // this can be done using LittleFS or SPIFFS for local storage
}

void connect_to_network()
{}

*/

void setup()
{
    pinMode(power_detector_pin, INPUT);
    create_network();
    Serial.begin(115200);
    Serial.println("Power Monitoring Hub started");
    Serial.println("IP Address: ");
    Serial.println(WiFi.softAPIP()):
}

void loop()
{
    int power_state = digitalRead(power_detector_pin);
    //put code here

}