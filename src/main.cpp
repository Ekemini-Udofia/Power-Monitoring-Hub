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
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define power_detector_pin 1// change this as needed
#define indicator_led 2 //for the led bulb. Change this when an external indicator bulb has been put 


const char* ssid = "Power Hub";
const char* password = "1234567890";

DNSServer default_server;
AsyncWebServer server(80);

String network_ssid;
String network_password;

bool recieved_ssid = false;
bool recieved_password = false;

const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">

    <head>
        <meta charset="UTF-8" />
        <meta name="viewport" content="width=device-width, initial-scale=1.0" />
        <title>Configure Device</title>
        <style>
                body {
                margin: 0;
                font-family: 'Poppins', sans-serif;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
                transition: background 0.3s, color 0.3s;
                background: var(--bg-color, linear-gradient(135deg, #f0f4f8, #d9e2ec));
                color: var(--text-color, #333);
            }

            .card {
                background: var(--card-bg, #ffffff);
                padding: 2rem;
                border-radius: 15px;
                box-shadow: 0 10px 20px rgba(0, 0, 0, 0.1);
                width: 100%;
                max-width: 400px;
                position: relative;
            }

            h2 {
                text-align: center;
                margin-bottom: 1.5rem;
                color: inherit;
            }

            label {
                display: block;
                margin-bottom: 0.5rem;
                font-weight: 500;
            }

            input {
                width: 90%;
                padding: 0.75rem 1rem;
                margin-bottom: 1.2rem;
                border: 1px solid #ccc;
                border-radius: 8px;
                font-size: 1rem;
                transition: border-color 0.3s;
                background: var(--input-bg, #fff);
                color: inherit;
            }

            input:focus {
                border-color: #007bff;
                outline: none;
            }

            .btn {
                width: 100%;
                padding: 0.75rem;
                background: #007bff;
                color: #fff;
                border: none;
                border-radius: 8px;
                font-size: 1rem;
                font-weight: 600;
                cursor: pointer;
                transition: background 0.3s;
            }

            .btn:hover {
                background: #0056b3;
            }

            .theme-toggle {
                position: absolute;
                top: 1rem;
                right: 1rem;
                font-size: 1.5rem;
                cursor: pointer;
                user-select: none;
            }

            /* Dark mode variables */
            body.dark-mode {
                --bg-color: linear-gradient(135deg, #121212, #1e1e1e);
                --card-bg: #2c2c2c;
                --text-color: #f0f0f0;
                --input-bg: #3a3a3a;
            }

            @media (max-width: 480px) {
                .card {
                    padding: 1.5rem;
                }
            }
        </style>
        <script>
            document.addEventListener('DOMContentLoaded', () => {
                const themeToggle = document.getElementById('themeToggle');
                const body = document.body;
        
                if (localStorage.getItem('theme') === 'dark') {
                    body.classList.add('dark-mode');
                    themeToggle.textContent = 'light';
                }
        
                themeToggle.addEventListener('click', () => {
                    body.classList.toggle('dark-mode');
                    const isDark = body.classList.contains('dark-mode');
                    themeToggle.textContent = isDark ? 'light' : 'dark';
                    localStorage.setItem('theme', isDark ? 'dark' : 'light');
                });
            });
        </script>
    </head>

    <body>

        <div class="card">
            <div class="theme-toggle" id="themeToggle">dark</div>
            <h2>Configure Device</h2>
            <form action="/submit" method="POST">
                <label for="ssid">Wi-Fi SSID</label>
                <input type="text" id="ssid" name="ssid" placeholder="Enter Wi-Fi SSID" required />

                <label for="password">Wi-Fi Password</label>
                <input type="password" id="password" name="password" placeholder="Enter Wi-Fi Password" required />

                <label for="deviceName">Device Name (optional)</label>
                <input type="text" id="deviceName" name="deviceName" placeholder="Enter Device Name" />

                <button class="btn" type="submit">Save & Connect</button>
            </form>
        </div>
        <script src="design.js" defer></script>
    </body>

    </html> 
    )rawliteral";

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


void create_web_server()
{
    // create a web server to serve the webpage
    // this can be done using the ESPAsyncWebServer library or the WebServer library

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", index_html);
        Serial.println("Client Connected");
    });

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
}

void log_power_data()
{
    // log the power state to a file or database
    // this can be done using LittleFS or SPIFFS for local storage
}

void connect_to_network()
{
    WiFi.begin(network_ssid.c_str(), network_password.c_str());
    Serial.println("Connecting to WiFi...");
    while(WiFi.status() != WL_CONNECTED) {
        Serial.println(".");
        delay(1000);
    }
}




void setup()
{
    pinMode(power_detector_pin, INPUT);
    Serial.begin(115200);
    Serial.println("Power Monitoring Hub started");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(ssid, password);
    Serial.println("AP IP Address: " + WiFi.softAPIP().toString());
    Serial.println("Starting Async Web server");
    create_web_server();
    Serial.println("Starting dns server");

    default_server.start(53, "*", WiFi.softAPIP());
    server.addHandler(new captive_request_handler()).setFilter(ON_AP_FILTER);
    server.begin();
    Serial.println("Web server started");
}

void loop()
{
    //int power_state = digitalRead(power_detector_pin);
    default_server.processNextRequest();
    if(recieved_ssid && recieved_password){
        Serial.println("WiFi Credentials Recieved");
        Serial.println("All done\n Restarting Device....");
        delay(15000);
    }

}