#ifndef CAPT_PORTAL_CONNECT
#define CAPT_PORTAL_CONNECT

// Inbuilt libs
#include <preferences.h>

// Project headers
#include "web_server.hpp"

bool connect_to_network() // Function 3
{
  bool connected = false;
  const unsigned startAttemptTime = millis();
  const unsigned long timeout = 10000; // 10 seconds
  WiFi.mode(WIFI_STA);
  WiFi.begin(network_ssid.c_str(), network_password.c_str());
  Serial.println("Connecting to WiFi"); // Debug
  connected = (WiFi.waitForConnectResult() == WL_CONNECTED) ? true : false;
  return connected;
}
void Initialize_and_connect()
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
        Serial.println("Failed to initialize and connect.\nEntering deep sleep mode to save power...");
        //enter deep sleep
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
bool is_internet_connected()
{
  bool is_internet = false;
  WiFiClient internet;
  if(internet.connect("google.com", 80))
  {
    is_internet = true;
  }
  return is_internet;
}
#endif