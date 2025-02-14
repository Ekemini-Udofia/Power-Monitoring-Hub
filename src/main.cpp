#include <Arduino.h>
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