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
#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h> 
#include <preferences.h>
// #include <Arduino_FreeRTOS.h>

// My personally defined headers
#include "capt_portal.hpp"

// Define the GPIO pins on the ESP32 different functions
#ifndef hdw_pins
#define power_detector_pin 1
#define indicator_led 2 
#endif



const char* default_ssid = "Power Hub";
const char* default_password = "1234567890";

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
// Don't touch the code before this yet. It works but we(I) don't know how for now
//
//
// Beginning Of planned code

bool connect_thru_captive_portal() // Function 2
{
  bool connected = false;
  //Captive portal created - Yes
  //Also scan for available networks and send it to client 
  WiFi.mode(WIFI_AP_STA);
  #define max_num_net 10
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

bool connect_to_network() // Function 3
{
  bool connected = false;
  WiFi.mode(WIFI_STA);
  WiFi.begin(network_ssid.c_str(), network_password.c_str());
  Serial.println("Connecting to WiFi..."); //for my debugging - delete later
  delay(10000);
  if(WiFi.status() == WL_CONNECTED)
  {
    connected = true;
  }
  else{
    connected = false;
  }
  return connected;
}
void Initialize_and_connect() // Function 1
{
  bool credentials_saved;
  Preferences is_cr_saved;
  is_cr_saved.begin("credentials", 0);
  credentials_saved = is_cr_saved.getUInt("is_cred_sv", 0);

  if(credentials_saved)
  {
    //connect to the previously saved network
    network_ssid = is_cr_saved.getString("ssid", "");
    network_password = is_cr_saved.getString("password", "");

    //write code for connecting to the network
    bool connected = connect_to_network();
    if(connected)
    {
      Serial.println("Connected to network"); // for my debugging - delete later
      //Dont bother re-saving the credentials
      /*is_cr_saved.putUInt("is_cred_sv", 0);
      is_cr_saved.putString("ssid", network_ssid);
      is_cr_saved.putString("password", network_password);
      */


      //continue to power logging
     
    }
    else
    {
      Serial.println("Failed to connect to network.\nStarting Captive Portal");
      bool connected = connect_thru_captive_portal();
      if(connected)
      {
        //save the credentials
        is_cr_saved.putUInt("is_cred_sv", 1);
        is_cr_saved.putString("ssid", network_ssid);
        is_cr_saved.putString("password", network_password);
        //Credentials have been saved
      }
      else
      {
        Serial.println("Failed to initialize and connect.\n Restarting device...");
        ESP.restart();
      }
    }
  }
  else
  {
    //start the captive portal
    bool connected = connect_thru_captive_portal();
    if(connected)
    {
      //save the credentials
      is_cr_saved.putUInt("is_cred_sv", 1);
      is_cr_saved.putString("ssid", network_ssid);
      is_cr_saved.putString("password", network_password);
      //Credentials have been saved
    }
    else
    {
      Serial.println("Failed to initialize and connect.\n Restarting device...");
      ESP.restart();
    }
  }


}


void log_power_data()
{
    // log the power state to a file or database
    // this can be done using LittleFS or SPIFFS for local storage
}





void setup()
{
    // pinMode(power_detector_pin, INPUT);
    Serial.begin(115200);
    Serial.println("Power Monitoring Hub started");
    

    Initialize_and_connect();
    // WiFi.softAP(default_ssid, default_password);

}

void loop()
{
    //int power_state = digitalRead(power_detector_pin);
    
}