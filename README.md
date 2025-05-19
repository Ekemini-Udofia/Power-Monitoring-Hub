# Power Monitoring Hub 🚨⚡

A smart ESP32-based solution to monitor power (NEPA) uptime and downtime in real time. This project detects when mains electricity is active in your house using a GPIO-connected relay or optocoupler and displays status updates on a beautiful live webpage.

---

## 📋 Features

- Detects mains power presence through a relay or optocoupler.
- Hosts a Wi-Fi access point and captive portal for first-time setup.
- Saves and reconnects to your home Wi-Fi automatically.
- Logs uptime/downtime events with timestamps.
- Serves a web dashboard for monitoring power events live.
- (Optional) Sends logs to a predictive AI (e.g. Gemini API) to forecast future power availability.

---

## 🚀 How It Works

1. **Detect Power Supply**  
   Uses a GPIO pin to detect if power is ON or OFF via relay/optocoupler.

2. **Debounce & Timestamp**  
   Filters out false triggers and logs accurate time of state changes using `millis()` or NTP.

3. **Store Events**  
   Logs events into memory or LittleFS in a JSON array format.

4. **Web Dashboard**  
   A local web server hosts a live dashboard showing power status and history.

5. **Optional: Real-time Updates**  
   Uses WebSockets or AJAX polling to show live status on the browser.

6. **Optional: AI Forecasting**  
   Sends historical logs to AI API (like Gemini) to predict next power changes.

---

## 📱 Captive Portal Setup

- When no known Wi-Fi is found, the ESP32 starts a captive portal (`Power Hub`) at IP `192.168.4.1`.
- Users can select a network and enter a password via a sleek web UI.
- On submission, it saves credentials and connects to the specified network.

---

## 🧰 Dependencies

Make sure to install the following libraries:

- `ESPAsyncWebServer`
- `AsyncTCP`
- `DNSServer`
- `WiFi`
- `Preferences`

Install via PlatformIO or Arduino Library Manager.

---

## 🛠️ Pin Configuration

| Name                 | Pin | Description                     |
|----------------------|-----|---------------------------------|
| `power_detector_pin` | 1   | Connect to relay/optocoupler    |
| `indicator_led`      | 2   | Optional LED indicator output   |

---

## 🧪 To-Do

- [ ] Add LittleFS or SPIFFS logging.
- [ ] Build the full logging + NTP timestamp feature.
- [ ] Create the full web dashboard with historical logs.
- [ ] Integrate predictive API hook to Gemini.

---

## 💡 Credits

Ekemini Udofia - linktr.ee/ekeminieudofia

---
