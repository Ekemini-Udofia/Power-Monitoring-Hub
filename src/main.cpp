#include <Arduino.h>
#include <SPI.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <RH_ASK.h>
#include <Callmebot_ESP32.h>

#define Rf_PIN 27
RH_ASK driver(Rf_PIN);

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

void setup() 
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    WiFi.setHostname(hostname);
    Serial.println("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED && millis() < 5000) {
        delay(1000);
        Serial.println("Connecting...");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());
    if(has_internet())
    {
        Serial.println("Internet Connection Available");
    }
    else
    {
        Serial.println("Internet Connection Unavailable");
    }
    if(!driver.init())
    {
        Serial.println("Failed to initialise RF Module");
    }
    else
    {
        Serial.println("RF Module Initialised");
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