# ESP32 Home Automation Data Server

## Overview

This project sets up an **ESP32-based home automation server** that collects sensor data from multiple devices via a **433MHz RF receiver** and sends it to a remote website (**ekemini.com**) over WiFi.

## Features

- **433MHz Data Reception**: Receives data from various sensors around the house.
- **WiFi Connectivity**: Connects to a home WiFi network.
- **HTTP Data Transmission**: Sends sensor data to a remote server via HTTP POST requests.
- **Real-time Data Updates**: The website fetches and displays incoming data dynamically.

## Hardware Requirements

- **ESP32 Dev Module**
- **433MHz RF Receiver Module (e.g., MX-RM-5V)**
- **Power Supply (e.g., USB 5V)**
- **Jumper Wires**

## Software Requirements

- **PlatformIO IDE**
- **Arduino Framework**
- **ESP32 WiFi and HTTP libraries**

## Setup Instructions

### 1. Connect ESP32 to WiFi

- Configure WiFi SSID and password in the firmware.
- Ensure ESP32 connects to the internet successfully.

### 2. Connect the 433MHz Receiver

- Wire the receiver's **Data** pin to an ESP32 GPIO.
- Power the module using ESP32's **3.3V or 5V** pin.
- Use an appropriate library to decode RF signals.

### 3. Process and Format Sensor Data

- Capture signals from the **433MHz receiver**.
- Extract sensor values and format them as JSON.

### 4. Send Data to the Remote Server

- Set up an HTTP POST request to website to display the data
- Include sensor data in the request body.
- Handle potential failures (e.g., Wi-Fi disconnection, server errors).

### 5. Display Data on Website

- Ensure the website receives and stores incoming data.
- Use JavaScript to dynamically update the UI with new sensor readings.

## Code Structure

```
/esp32-home-server
|-- src/
|   |-- main.cpp   # ESP32 firmware (WiFi, RF Receiver, HTTP Client)
|-- include/
|-- platformio.ini # PlatformIO configuration
|-- README.md      # Project documentation
```

## Future Improvements

- Add **security features** like API authentication.
- Support **multiple sensor types** beyond RF-based ones.

## License

This project is open-source under the MIT License.

## Author

**Ekemini Udofia** –  linktr.ee/ekeminieudofia

