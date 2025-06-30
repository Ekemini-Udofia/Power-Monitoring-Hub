# ⚡ Power Monitoring Hub

A battery-powered ESP32-based device that detects when electrical power is restored (NEPA brings light) and logs the event. It connects to WiFi via a captive portal and sends the event to a remote server or webhook. Designed for energy efficiency using deep sleep, and built for anyone to use — no hardcoded credentials required.

---

## 📦 Features

- 🕵️ Detects power presence using a GPIO pin (relay)
- 🌙 Enters deep sleep when no power is present to conserve battery
- 📡 Wakes and connects to WiFi (via saved credentials or captive portal)
- 🕒 Syncs real-time clock using NTP
- 📤 Sends power-on timestamp to remote webhook/API (e.g., Pipedream, Firebase)
- 🌍 Optionally logs data locally (SPIFFS/LittleFS)
- ✅ No hardcoded credentials — user configures WiFi through a web interface

---

## 🚀 How It Works

1. On boot, ESP32 checks for power using a GPIO pin.
2. If power is off, it enters deep sleep and waits for a HIGH signal to wake.
3. If power is on, it:
   - Connects to WiFi (saved credentials or captive portal)
   - Syncs time via NTP
   - Logs the power-on event with a timestamp
   - Sends the data to a configured webhook or endpoint
4. Returns to deep sleep to conserve battery.

---

## 🛠 Hardware Required

| Component                 | Purpose                             |
|---------------------------|-------------------------------------|
| ESP32 Dev Module          | Core microcontroller                |
| LiPo Battery / Power Bank | Power source                        |
| Optocoupler / Relay       | Detect power state on input pin     |
| Push Button (optional)    | Enter WiFi config mode manually     |   

---

## 📲 WiFi Configuration (Captive Portal)

When first powered:
- Device creates a WiFi hotspot (`Power Hub / 1234567890`)
- Connect to this AP with a phone or PC
- Enter your home WiFi credentials via the web interface
- ESP32 saves and reuses this config on the next boot

---

## 📡 Remote Logging

This project supports sending event data to a remote endpoint.

### Example Payload:
```json
{
  "timestamp": "2025-06-30T09:45:00",
  "event": "POWER_ON",
  "device": "ESP32-HUB-01"
}
