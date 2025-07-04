#ifndef WEB_SERVER
#define WEB_SERVER

#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h> 
#include "capt_portal_page.hpp"

DNSServer default_server;
AsyncWebServer server(80);

String network_ssid;
String network_password;

bool recieved_ssid = false;
bool recieved_password = false;

// Global variable to store last scanned networks
std::vector<String> scanned_networks;

// Add this function to scan and update the global list
void scan_and_update_networks() {
    scanned_networks.clear();
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
        scanned_networks.push_back(WiFi.SSID(i));
    }
}



// captive handler class      
class captive_request_handler : public AsyncWebHandler
{
    public:
        captive_request_handler() {}
        virtual ~captive_request_handler() {}

        bool canHandle(AsyncWebServerRequest *request){
            return true;
        }

        void Handle_request(AsyncWebServerRequest *request){
            request->send(200, "text/html", index_html);
        }
};


bool connect_thru_captive_portal() 
{
  bool connected = false;
  //Captive portal created - Yes
  //Also scan for available networks and send it to client 
  WiFi.mode(WIFI_AP_STA);
  constexpr int max_num_net = 10;
  std::string network_list[max_num_net];
  int num_nets = WiFi.scanNetworks();
  if (num_nets == 0) {
        Serial.println("No networks found");
    } else {
        Serial.println("Available networks:");
        int count = min(num_nets, max_num_net); 
        for (int i = 0; i < count; i++) {
            network_list[i] = WiFi.SSID(i).c_str(); 
            Serial.printf("%d: %s (%d dBm)\n", i + 1, network_list[i].c_str(), WiFi.RSSI(i));
        }
    }

    //send the array to the web page 



  //send the list of available networks to the client

  // serve index_html page
  WiFi.softAP(default_ssid, default_password);

  Serial.println("AP IP Address: " + WiFi.softAPIP().toString());

  Serial.println("Starting Async Web server");
   // create_web_server();
  Serial.println("Starting dns server");default_server.start(53, "*", WiFi.softAPIP());
  scan_and_update_networks();
  server.addHandler(new captive_request_handler()).setFilter(ON_AP_FILTER);
  server.begin();
  Serial.println("Web server started");


  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/html", index_html);
      Serial.println("Client Connected");
  });

  server.on("/rescan", HTTP_GET, [](AsyncWebServerRequest *request){
    scan_and_update_networks();
    request->send(200, "text/plain", "rescanned");
  });

  //for reciveing the ssid and password
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request){
        String input_cred1;
        String input_param;

        if(request->hasParam("ssid")) {
            input_cred1 = request->getParam("ssid")->value();
            input_param = "ssid";
            network_ssid = input_cred1;
            Serial.println("SSID: " + input_cred1);
            recieved_ssid = true;
        }
        if(request->hasParam("password")) {
            input_cred1 = request->getParam("password")->value();
            input_param = "password";
            network_password = input_cred1;
            Serial.println("Password: " + input_cred1);
            recieved_password = true;
        }

        request->send(200, "text/html", "Thank you!");
  });

  //for the scanning icon in the capture portal page
  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "[";
    for (size_t i = 0; i < scanned_networks.size(); ++i) {
        json += "\"" + scanned_networks[i] + "\"";
        if (i < scanned_networks.size() - 1) json += ",";
    }
    json += "]";
    request->send(200, "application/json", json);
  });
  // Send back ssid and password
  server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
        network_ssid = request->getParam("ssid", true)->value();
        network_password = request->getParam("password", true)->value();
        recieved_ssid = true;
        recieved_password = true;
        Serial.println("SSID: " + network_ssid);
        Serial.println("Password: " + network_password);
        request->send(200, "text/plain", "OK");
    } else {
        request->send(400, "text/plain", "Missing parameters");
    }
  });

  Serial.println("Waiting for user to connect to AP and submit WiFi credentials...");
  unsigned long startTime = millis();
  const unsigned long timeout = 300000; // 5 minutes before it disconnects
  while (!(recieved_ssid && recieved_password) && (millis() - startTime < timeout)) {
    delay(500);
    Serial.print(".");
  }

  if(recieved_ssid && recieved_password){
      Serial.println("WiFi Credentials Recieved");
      Serial.println("Connecting to network...");
      WiFi.mode(WIFI_STA);
      WiFi.begin(network_ssid.c_str(), network_password.c_str());

      // Wait for esp to connect and add timeout
      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
        delay(500);
        Serial.print(".");
      }

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to WiFi!");
        connected = true;
        WiFi.softAPdisconnect(true);
        Serial.println("Stage 1 Passed");
      } else {
        Serial.println("Failed to connect to WiFi.");
        connected = false;
        Serial.println("Stage 1 Failed");
      }
  }

  Serial.println("Disconnecting AP");
  default_server.stop();
  server.end();
  Serial.println("Stopping server");  

  return connected;

}
void CONFIG()
{
  connect_thru_captive_portal();
}
#endif