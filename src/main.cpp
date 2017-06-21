#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <Adafruit_MQTT.h> // Include this library for securely connecting to the internet using WiFi.
#include <Adafruit_MQTT_Client.h> // Include this library for MQTT communication.
#include <FS.h> // Include this library for access to the ESP8266's file system.
#include <Streaming.h> // Include this library for using the << Streaming operator.

#include <Configuration.h>
#include <Communication.h>
#include <PlantCare.h>

Configuration configuration;
Communication communication( &configuration );
PlantCare plantCare( &communication );

void setup()
{
    configuration.setup();
    communication.setup();
    plantCare.setup();
}

void loop()
{
    plantCare.takeCareOfPlant();
}