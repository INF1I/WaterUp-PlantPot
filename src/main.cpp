#include "Arduino.h"
#include <ESP8266WiFi.h>

WiFiServer server(80); //Initialize the server on Port 80

void setup() {

    WiFi.mode(WIFI_AP); //Our ESP8266-12E is an AccessPoint
    WiFi.softAP("Hello_IoT", "12345678"); // Provide the (SSID, password); .
    server.begin(); // Start the HTTP Server

}

void loop() { }