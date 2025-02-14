#include <Arduino.h>
#include <SPI.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

const char* ssid = "ekeminiudofia";
const char* password = "ekeminietuk";

void setup() 
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && millis() < 10000) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void loop() 
{

}