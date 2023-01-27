/*
======================================================================================================================
# Name           : ESP32-WebServer-Project-BME280.ino
# Author         : Mr. Mohamed Ali Haoufa
# Created on     : May 15, 2021
# Description    : It is the main program of the Automatic Wireless Mini Weather Station where we read the essential 
                   information from the BME280 sensor and send it to the ESP32 web server interface.
========================================================================================================================
*/

// Import required libraries

#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include "WiFi.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Replace with the network credentials so that ESP32 can connect to our local network.
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_WIFI_PASSWORD";

Adafruit_BME280 bme; // I2C

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// We’ve created three functions: one to read the temperature (readBME280Temperature()), the other to read humidity (readBME280Humidity()) and the last one to read pressure (readBME280Pressure()):
String readBME280Temperature() {
  // Read temperature as Celsius (the default).
  // Getting the temperature readings from the BME280 sensor :
  float t = bme.readTemperature();  

  // Convert temperature to Fahrenheit : 
  // t = 1.8 * t + 32;
  
  // Check if any reads failed and exit early (to try again) :
  if (isnan(t)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "--";            // a condition that returns two dashes (–-) in case the sensor fails to get the readings.
  }
  else {
    Serial.print("temperture :");
    Serial.println(t);
    return String(t);       //The readings are returned as string type. To convert a float to a string, use the String() function.
  }
}

String readBME280Humidity() {
  float h = bme.readHumidity();   // Getting the humidity readings from the BME280 sensor
  if (isnan(h)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "--";
  }
  else {
   Serial.print("humidity :");
    Serial.println(h);
    return String(h);
  }
}

String readBME280Pressure() {
  float p = bme.readPressure()/ 100.0F;   // Getting the Pressure readings from the BME280 sensor
  if (isnan(p)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "--";
  }
  else {
   Serial.print("pressure :");
    Serial.println(p);
    return String(p);
  }
}

String processor(const String& var) {
  // Serial.println(var);
  if (var == "TEMPERATURE") {
    // When the web page is requested, we check if the HTML has any placeholders.
    // If it finds the %TEMPERATURE% placeholder, we return the temperature by calling the readBME280Temperature() function created previously.
    return readBME280Temperature();
  }
  else if (var == "HUMIDITY") {
    return readBME280Humidity();
  }
  else if (var == "PRESSURE") {
    return readBME280Pressure();
  }
  return String();
}


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  bool status; 
  // default settings
  // (We can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page (Handle requests)

  // When ESP32 receives a request on the root URL, we send the HTML text that is saved in SPIFFS under the "index.html" name 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  
  // if we receive a request on the /temperature URL, we call the readBME280Temperature() function that returns the updated temperature value.
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readBME280Temperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readBME280Humidity().c_str());
  });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readBME280Pressure().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
