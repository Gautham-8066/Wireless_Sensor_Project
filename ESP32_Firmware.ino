#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

// --- WI-FI CREDENTIALS ---
const char* ssid     = "yourssid";
const char* password = "yourpassword";

// --- PIN DEFINITIONS ---
// BME680 Pins
#define SDA_PIN 18
#define SCL_PIN 19
// CS_PIN give 3.3v directly from an external source
// pH Sensor Pin
#define PH_PIN 34

// --- GLOBAL OBJECTS ---
WiFiServer server(80);
Adafruit_BME680 bme;

// --- pH CALIBRATION ---
float neutralVoltage = 2.50;   // pH 7 voltage
float acidVoltage    = 3.00;   // pH 4 voltage

void setup() {
  Serial.begin(115200);
  delay(500);

  // 1. Configure ADC (Crucial for pH on ESP32)
  analogSetAttenuation(ADC_11db); 

  // 2. Start I2C for BME680
  Wire.begin(SDA_PIN, SCL_PIN, 100000);

  // 3. Initialize BME680
  Serial.println("Initializing sensors...");
  if (!bme.begin(0x76, &Wire)) {
    if (!bme.begin(0x77, &Wire)) {
      Serial.println("Error: BME680 not found! Check wiring.");
      while (1);
    }
  }

  // BME Configuration
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);

  // 4. Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // <--- Update Python script with this IP!
  
  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Listen for Python script

  if (client) {
    Serial.println("Client Connected.");
    
    while (client.connected()) {
      
      // --- 1. READ pH SENSOR ---
      unsigned long total = 0;
      // Average 10 readings to smooth out the noise
      for(int i=0; i<10; i++) {
        total += analogRead(PH_PIN);
        delay(10);
      }
      int adcValue = total / 10;
      float voltage = adcValue * (3.3 / 4095.0); 
      
      float slope = (7.0 - 4.0) / (neutralVoltage - acidVoltage);
      float intercept = 7.0 - (slope * neutralVoltage);
      float pHValue = slope * voltage + intercept;

      // --- 2. READ BME680 SENSOR ---
      // We initialize empty strings in case reading fails
      String bmeData = "BME Read Failed"; 
      
      if (bme.performReading()) {
         bmeData = "Temp: " + String(bme.temperature) + " C | " +
                   "Hum: " + String(bme.humidity) + " % | " +
                   "Pres: " + String(bme.pressure / 100.0) + " hPa | " +
                   "Gas: " + String(bme.gas_resistance / 1000.0) + " KOhms";
      }

      // --- 3. COMBINE DATA ---
      String phData = "pH: " + String(pHValue, 2) + " (" + String(voltage, 2) + "V)";
      
      String finalOutput = ">> " + phData + " || " + bmeData;

      // --- 4. SEND DATA ---
      client.println(finalOutput);  // To Python
      Serial.println(finalOutput);  // To Serial Monitor (Debug)

      delay(2000); // Wait 2 seconds before next reading
    }
    
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
