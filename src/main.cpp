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


// Include libraries and define constants and stuff

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h> 
#include <preferences.h>


#define power_detector_pin 1// change this as needed
#define indicator_led 2 //for the led bulb. Change this when an external indicator bulb has been put 



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

//capture portal page

 const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Power Hub Setup</title>
    <style>
      /* Base styles */
      * {
        box-sizing: border-box;
        margin: 0;
        padding: 0;
        font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      }

      body {
        background: linear-gradient(135deg, #9b87f5 0%, #6E59A5 100%);
        min-height: 100vh;
        display: flex;
        align-items: center;
        justify-content: center;
        padding: 1rem;
        color: #333;
      }

      /* Card styles */
      .card {
        background: white;
        border-radius: 0.5rem;
        width: 100%;
        max-width: 28rem; /* 448px */
        box-shadow: 0 10px 25px rgba(0, 0, 0, 0.2);
        overflow: hidden;
      }

      .card-header {
        padding: 1.5rem 1.5rem 1rem;
        text-align: center;
      }

      .card-icon {
        color: #9b87f5;
        font-size: 2rem;
        margin-bottom: 1rem;
        display: flex;
        justify-content: center;
      }

      .card-title {
        font-size: 1.5rem;
        font-weight: 700;
        margin-bottom: 0.5rem;
      }

      .card-description {
        color: #666;
        margin-bottom: 0.5rem;
        font-size: 0.875rem;
      }

      .card-content {
        padding: 1.5rem;
      }

      .card-footer {
        padding: 1rem 1.5rem;
        text-align: center;
        color: #666;
        font-size: 0.875rem;
        border-top: 1px solid #eee;
      }

      .card-footer p {
        margin-bottom: 0.5rem;
      }

      .card-footer p:last-child {
        font-size: 0.75rem;
        color: #888;
      }

      /* Form styles */
      .form {
        display: flex;
        flex-direction: column;
        gap: 1.25rem;
      }

      .form-group {
        display: flex;
        flex-direction: column;
        gap: 0.5rem;
      }

      .label {
        font-weight: 500;
        font-size: 0.875rem;
      }

      .input-wrapper {
        position: relative;
      }

      .input {
        width: 100%;
        padding: 0.625rem 0.75rem;
        padding-left: 2.5rem;
        border: 1px solid #ddd;
        border-radius: 0.375rem;
        font-size: 1rem;
        outline: none;
        transition: border-color 0.2s;
      }

      .input:focus {
        border-color: #9b87f5;
        box-shadow: 0 0 0 2px rgba(155, 135, 245, 0.2);
      }

      .input:disabled {
        background-color: #f5f5f5;
        cursor: not-allowed;
        opacity: 0.7;
      }

      .input-icon {
        position: absolute;
        left: 0.75rem;
        top: 50%;
        transform: translateY(-50%);
        color: #999;
        display: flex;
        align-items: center;
        pointer-events: none;
      }

      .password-toggle {
        position: absolute;
        right: 0.75rem;
        top: 50%;
        transform: translateY(-50%);
        background: transparent;
        border: none;
        color: #666;
        cursor: pointer;
        display: flex;
        padding: 0;
      }

      /* Button styles */
      .btn {
        display: flex;
        justify-content: center;
        align-items: center;
        gap: 0.5rem;
        background-color: #9b87f5;
        color: white;
        border: none;
        border-radius: 0.375rem;
        padding: 0.625rem 1rem;
        font-weight: 500;
        cursor: pointer;
        transition: background-color 0.2s;
      }

      .btn-outline {
        background-color: transparent;
        border: 1px solid #ddd;
        color: #666;
      }

      .btn:hover:not(:disabled) {
        background-color: #8a74f2;
      }

      .btn-outline:hover:not(:disabled) {
        background-color: #f5f5f5;
      }

      .btn:disabled {
        opacity: 0.7;
        cursor: not-allowed;
      }

      .btn-icon {
        display: inline-flex;
        align-items: center;
        justify-content: center;
        padding: 0.5rem;
      }

      .btn-link {
        background: none;
        border: none;
        color: #9b87f5;
        padding: 0;
        font-size: 0.75rem;
        text-decoration: underline;
        cursor: pointer;
      }

      .btn-link:hover {
        color: #8a74f2;
      }

      /* Select styles */
      .select-wrapper {
        position: relative;
      }

      .select {
        width: 100%;
        padding: 0.625rem 0.75rem;
        border: 1px solid #ddd;
        border-radius: 0.375rem;
        font-size: 1rem;
        appearance: none;
        background-color: white;
        cursor: pointer;
      }

      .select:focus {
        border-color: #9b87f5;
        outline: none;
        box-shadow: 0 0 0 2px rgba(155, 135, 245, 0.2);
      }

      .select:disabled {
        background-color: #f5f5f5;
        cursor: not-allowed;
        opacity: 0.7;
      }

      /* Network selector */
      .network-selector {
        display: flex;
        gap: 0.5rem;
      }

      .network-selector .select-wrapper {
        flex: 1;
      }

      .manual-input {
        margin-top: 0.5rem;
        display: flex;
        flex-direction: column;
        gap: 0.5rem;
      }

      .scanning-message {
        display: flex;
        align-items: center;
        gap: 0.5rem;
        color: #666;
        font-size: 0.75rem;
        margin-top: 0.5rem;
      }

      .back-link {
        text-align: right;
        margin-top: 0.5rem;
      }

      /* Status and alerts */
      .alert {
        padding: 0.75rem;
        border-radius: 0.375rem;
        margin-top: 1.25rem;
        display: flex;
        align-items: center;
        gap: 0.5rem;
      }

      .alert-connecting {
        background-color: #e6f0ff;
        color: #1a56db;
        border: 1px solid #c2d8ff;
      }

      .alert-connected {
        background-color: #ecfdf5;
        color: #0a7f5c;
        border: 1px solid #d1fae5;
      }

      .connection-dot {
        height: 10px;
        width: 10px;
        border-radius: 50%;
        display: inline-block;
      }

      .connected {
        background-color: #10b981;
      }

      /* Animations */
      @keyframes spin {
        to {
          transform: rotate(360deg);
        }
      }

      @keyframes pulse {
        0%, 100% {
          opacity: 0.5;
        }
        50% {
          opacity: 1;
        }
      }

      .spin {
        animation: spin 1s linear infinite;
      }

      .pulse {
        animation: pulse 1.5s infinite;
      }

      /* SVG icons */
      .icon {
        width: 20px;
        height: 20px;
        display: inline-block;
        vertical-align: middle;
      }

      .icon-sm {
        width: 16px;
        height: 16px;
      }
    </style>
  </head>
  <body>
    <div class="card">
      <div class="card-header">
        <div class="card-icon">
          <!-- WiFi Icon -->
          <svg xmlns="http://www.w3.org/2000/svg" class="icon" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M5 13a10 10 0 0 1 14 0"/><path d="M8.5 16.5a5 5 0 0 1 7 0"/><path d="M2 8.82a15 15 0 0 1 20 0"/><line x1="12" x2="12.01" y1="20" y2="20"/></svg>
        </div>
        <h1 class="card-title">Power Hub Setup</h1>
        <p class="card-description">Connect this device to your home WiFi network</p>
      </div>
      
      <div class="card-content">
        <form id="wifi-form" class="form">
          <div class="form-group">
            <label for="network" class="label">WiFi Network</label>
            <div id="network-container">
              <!-- Network selector will be rendered here -->
            </div>
          </div>
          
          <div class="form-group">
            <label for="password" class="label">Password</label>
            <div class="input-wrapper">
              <div class="input-icon">
                <!-- Lock Icon -->
                <svg xmlns="http://www.w3.org/2000/svg" class="icon-sm" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect width="18" height="11" x="3" y="11" rx="2" ry="2"/><path d="M7 11V7a5 5 0 0 1 10 0v4"/></svg>
              </div>
              <input type="password" id="password" class="input" placeholder="WiFi password" required disabled>
              <button type="button" class="password-toggle" id="toggle-password" disabled>
                <!-- Eye Icon -->
                <svg xmlns="http://www.w3.org/2000/svg" class="icon-sm" id="eye-icon" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M2 12s3-7 10-7 10 7 10 7-3 7-10 7-10-7-10-7Z"/><circle cx="12" cy="12" r="3"/></svg>
              </button>
            </div>
          </div>
          
          <button type="submit" id="connect-btn" class="btn" disabled>Connect</button>
          
          <div id="connection-status"></div>
        </form>
      </div>
      
      <div class="card-footer">
        <p>This device needs internet access to function properly.</p>
        <p>Your credentials are only stored locally on this device.</p>
      </div>
    </div>

    <script>
      // State management
      const state = {
        ssid: "",
        password: "",
        showPassword: false,
        availableNetworks: [],
        scanning: true,
        manualInput: false,
        connectionStatus: 'disconnected', // 'disconnected', 'connecting', 'connected'
      };

      // DOM References
      const networkContainer = document.getElementById('network-container');
      const passwordInput = document.getElementById('password');
      const connectBtn = document.getElementById('connect-btn');
      const togglePasswordBtn = document.getElementById('toggle-password');
      const eyeIcon = document.getElementById('eye-icon');
      const wifiForm = document.getElementById('wifi-form');
      const connectionStatusEl = document.getElementById('connection-status');

      // Initialize
      document.addEventListener('DOMContentLoaded', () => {
        renderNetworkSelector();
        setupEventListeners();
        scanNetworks();
      });

      // Event Listeners
      function setupEventListeners() {
        togglePasswordBtn.addEventListener('click', togglePasswordVisibility);
        wifiForm.addEventListener('submit', handleConnect);
        passwordInput.addEventListener('input', updateButtonState);
      }

      // Toggle password visibility
      function togglePasswordVisibility() {
        state.showPassword = !state.showPassword;
        passwordInput.type = state.showPassword ? 'text' : 'password';
        
        if (state.showPassword) {
          eyeIcon.innerHTML = '<path d="M9.88 9.88a3 3 0 1 0 4.24 4.24"></path><path d="M10.73 5.08A10.43 10.43 0 0 1 12 5c7 0 10 7 10 7a13.16 13.16 0 0 1-1.67 2.68"></path><path d="M6.61 6.61A13.526 13.526 0 0 0 2 12s3 7 10 7a9.74 9.74 0 0 0 5.39-1.61"></path><line x1="2" x2="22" y1="2" y2="22"></line>';
        } else {
          eyeIcon.innerHTML = '<path d="M2 12s3-7 10-7 10 7 10 7-3 7-10 7-10-7-10-7Z"/><circle cx="12" cy="12" r="3"/>';
        }
      }

      // Network scanning
      function scanNetworks() {
        state.scanning = true;
        state.availableNetworks = [];
        renderNetworkSelector();
        
        // In a real implementation, we would make an API call to the ESP32
        fetch('/scan')
          .then(response => response.json())
          .then(networks => {
            state.availableNetworks = networks;
            state.scanning = false;
            renderNetworkSelector();
          })
          .catch(() => {
            // If API call fails, set empty networks for demo
            setTimeout(() => {
              state.availableNetworks = [];
              state.scanning = false;
              renderNetworkSelector();
            }, 1500);
          });
      }

      // Render Network Selector
      function renderNetworkSelector() {
        if (state.manualInput) {
          networkContainer.innerHTML = `
            <div class="manual-input">
              <input type="text" id="manual-network" class="input" 
                    value="${state.ssid}" placeholder="Enter WiFi name" 
                    ${state.connectionStatus === 'connecting' ? 'disabled' : ''}>
              <div class="back-link">
                <button type="button" class="btn-link" id="back-to-list">
                  Back to network list
                </button>
              </div>
            </div>
          `;
          
          document.getElementById('manual-network').addEventListener('input', (e) => {
            state.ssid = e.target.value;
            updateButtonState();
          });
          
          document.getElementById('back-to-list').addEventListener('click', () => {
            state.manualInput = false;
            state.ssid = '';
            renderNetworkSelector();
            scanNetworks();
          });
          
        } else {
          networkContainer.innerHTML = `
            <div class="network-selector">
              <div class="select-wrapper" style="flex: 1;">
                <select id="network-select" class="select" 
                        ${state.scanning || state.connectionStatus === 'connecting' ? 'disabled' : ''}>
                  <option value="" selected disabled>
                    ${state.scanning ? 'Scanning...' : 'Select network...'}
                  </option>
                  ${state.availableNetworks.length > 0 ? 
                    state.availableNetworks.map(network => 
                      `<option value="${network}">${network}</option>`
                    ).join('') : 
                    `<option value="manual-input">No networks found - Enter manually</option>`
                  }
                </select>
              </div>
              <button type="button" id="scan-btn" class="btn btn-outline btn-icon" 
                      ${state.scanning || state.connectionStatus === 'connecting' ? 'disabled' : ''}>
                <!-- Refresh Icon -->
                <svg xmlns="http://www.w3.org/2000/svg" class="icon-sm ${state.scanning ? 'spin' : ''}" 
                    width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" 
                    stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                  <path d="M3 12a9 9 0 1 0 9-9 9.75 9.75 0 0 0-6.74 2.74L3 8"/>
                  <path d="M3 3v5h5"/>
                </svg>
              </button>
            </div>
            ${state.scanning ? `
              <div class="scanning-message">
                <!-- Loader Icon -->
                <svg xmlns="http://www.w3.org/2000/svg" class="icon-sm spin" 
                    width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" 
                    stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                  <path d="M21 12a9 9 0 1 1-6.219-8.56"/>
                </svg>
                Scanning for networks...
              </div>
            ` : ''}
          `;
          
          const networkSelect = document.getElementById('network-select');
          const scanBtn = document.getElementById('scan-btn');
          
          if (networkSelect) {
            networkSelect.addEventListener('change', (e) => {
              if (e.target.value === 'manual-input') {
                state.manualInput = true;
                state.ssid = '';
                renderNetworkSelector();
              } else {
                state.ssid = e.target.value;
                updateButtonState();
              }
            });
          }
          
          if (scanBtn) {
            scanBtn.addEventListener('click', scanNetworks);
          }
        }
        
        updateButtonState();
      }

      // Update Password Input State
      function updatePasswordState() {
        const isDisabled = state.connectionStatus === 'connecting' || !state.ssid;
        passwordInput.disabled = isDisabled;
        togglePasswordBtn.disabled = isDisabled;
        // Optionally, clear password if disabled
        if (isDisabled) passwordInput.value = '';
      }

      // Update Connect Button State
      function updateButtonState() {
        updatePasswordState();
        connectBtn.disabled = state.connectionStatus === 'connecting' || !state.ssid || !passwordInput.value;
        
        // Update connect button text based on connection status
        if (state.connectionStatus === 'connecting') {
          connectBtn.innerHTML = `
            <svg xmlns="http://www.w3.org/2000/svg" class="icon-sm spin" 
                width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" 
                stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <path d="M21 12a9 9 0 1 1-6.219-8.56"/>
            </svg>
            Connecting...
          `;
        } else {
          connectBtn.textContent = 'Connect';
        }
      }

      // Handle form submission
      function handleConnect(e) {
        e.preventDefault();
        
        if (state.ssid && passwordInput.value) {
          state.password = passwordInput.value;
          state.connectionStatus = 'connecting';
          updateButtonState();
          renderConnectionStatus();
          
          // In a real implementation, this would be an API call to the ESP32
          const formData = new FormData();
          formData.append('ssid', state.ssid);
          formData.append('password', state.password);
          
          fetch('/connect', {
            method: 'POST',
            body: formData
          })
            .then(() => {
              setTimeout(() => {
                state.connectionStatus = 'connected';
                updateButtonState();
                renderConnectionStatus();
              }, 2000);
            })
            .catch(error => {
              console.error('Connection error:', error);
              state.connectionStatus = 'disconnected';
              updateButtonState();
              renderConnectionStatus();
            });
        }
      }

      // Render connection status
      function renderConnectionStatus() {
        if (state.connectionStatus === 'disconnected') {
          connectionStatusEl.innerHTML = '';
          return;
        }

        if (state.connectionStatus === 'connecting') {
          connectionStatusEl.innerHTML = `
            <div class="alert alert-connecting">
              <svg xmlns="http://www.w3.org/2000/svg" class="icon-sm spin" 
                  width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" 
                  stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <path d="M21 12a9 9 0 1 1-6.219-8.56"/>
              </svg>
              Connecting to network...
            </div>
          `;
        } else if (state.connectionStatus === 'connected') {
          connectionStatusEl.innerHTML = `
            <div class="alert alert-connected">
              <span class="connection-dot connected"></span>
              Connected successfully! Device is now online.
            </div>
          `;
        }
      }
    </script>
  </body>
  </html>
      )rawliteral";

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