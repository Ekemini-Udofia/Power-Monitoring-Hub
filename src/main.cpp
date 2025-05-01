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

#define power_detector_pin 27


const char* ssid = "ekeminiudofia";
const char* password = "ekeminietuk"; 
const char* hostname = "Home Server";

bool has_internet()
{
    WiFiClient client;
    if(client.connect("www.google.com", 80, 2000))
    {// attempt to connect to google.com
        client.stop();
        return true; // if the connection is succesful
    }
    else
    {
        client.stop();
        return false; // the connection isnt succesful
    }
}

void WiFiConnect()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.setHostname(hostname);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());
}


void setup() 
{
    Serial.begin(115200);
    Serial.println("Connecting to WiFi");
    WiFiConnect();

    if(!driver.init())
    {
        Serial.println("Failed to initialise RF Module");
    }
    else
    {
        Serial.println("RF Module Initialised");
    }
    if(has_internet())
    {
        Serial.println("Internet Connection Available");
    }
    else
    {
        Serial.println("Internet Connection Unavailable");
        WiFiConnect();
    }


}

void loop() 
{
    uint8_t buffer[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t length_of_messagefer = sizeof(buffer);

    if(driver.recv(buffer, &length_of_messagefer))
    {
        Serial.println("Message Received");
        Serial.println((char*)buffer);
        delay(10);
    }
    Callmebot.whatsappMessage("+2349134102038", "6289833", String((char*)buffer));
    Callmebot.whatsappMessage("+2349134102038", "6289833", "Test");
    Serial.println("Message Sent");

}