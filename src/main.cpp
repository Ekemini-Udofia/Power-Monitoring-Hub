#include <Arduino.h>
#include <RH_ASK.h>
#include <SPI.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

RH_ASK driver;
AsyncWebServer server(80);

String receivedData = "";
const char* ssid = "ekeminiudofia";
const char* password = "ekeminietuk";

void setup() {
    Serial.begin(115200);
    if (!driver.init()) {
        Serial.println("Failed to initialize driver");
    }

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && millis() < 10000) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Set up web server
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", "<html><body><h1>Data: " + receivedData + "</h1></body></html>");
    });
    server.begin();
}

void loop() {
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) {
        receivedData = String((char*)buf);
        Serial.println("Received: " + receivedData);
    }
}