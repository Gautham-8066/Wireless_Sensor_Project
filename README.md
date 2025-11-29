# Wireless pH & Environmental Monitor (ESP32 + Python)

This project is a wireless data acquisition system that monitors water quality (pH) and environmental metrics (Temperature, Humidity, Pressure, Gas Resistance). It uses an ESP32 to read sensor data and transmits it over WiFi (TCP/IP) to a Python client script running on a computer.

## 🛠️ Hardware Used
* **Microcontroller:** ESP32 Development Board
* **Environmental Sensor:** BME680 (I2C)
* **Liquid Sensor:** Analog pH Sensor (Generic)
* **Power:** 3.3V / 5V source

## 🔌 Wiring Connections

| Sensor | ESP32 Pin | Note |
| :--- | :--- | :--- |
| **BME680 SDA** | GPIO 18 | I2C Data |
| **BME680 SCL** | GPIO 19 | I2C Clock |
| **BME680 VCC** | 3.3V | |
| **BME680 GND** | GND | |
| **pH Signal** | GPIO 34 | Analog Input (ADC1) |
| **pH VCC/GND** | 5V/GND | Check your specific sensor voltage |

> **Note:** The pH sensor is connected to **GPIO 34** because it is an input-only pin connected to ADC1, which is safe to use while WiFi is active.

## 💻 Software Setup

### 1. ESP32 Firmware
1.  Open the `.ino` file in Arduino IDE.
2.  Install the required libraries via Library Manager:
    * `Adafruit BME680 Library`
    * `Adafruit Unified Sensor`
3.  **Important:** Update the `ssid` and `password` variables with your WiFi credentials.
4.  Upload the code to your ESP32.
5.  Open the Serial Monitor (Baud 115200) to find the **IP Address**.

### 2. Python Client
1.  Make sure you have Python installed.
2.  Open the Python script.
3.  Update the `ESP_IP` variable to match the IP address shown in the Arduino Serial Monitor.
4.  Run the script:
    ```bash
    python client.py
    ```

## 📊 How it Works
1.  The ESP32 connects to WiFi and starts a TCP Server on Port 80.
2.  It reads the pH voltage (calibrated with neutral/acid points) and BME680 data.
3.  The Python script connects to the ESP32 IP address.
4.  The ESP32 streams the formatted data string to the Python console every 2 seconds.
