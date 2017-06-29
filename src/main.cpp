/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Created: 19-06-2017 17:51
 * Licence: GPLv3 - General Public Licence version 3
 */
#include <Arduino.h> // Include this library for using basic system functions and variables.
#include <ESP8266WiFi.h> // Include this library for working with the ESP8266 chip.
#include <WiFiManager.h> // Include this library for dynamically setting up the WiFi connection.
#include <Adafruit_MQTT.h> // Include this library for securely connecting to the internet using WiFi.
#include <Adafruit_MQTT_Client.h> // Include this library for MQTT communication.
#include <FS.h> // Include this library for access to the ESP8266's file system.*/
#include <Streaming.h> // Include this library for using the << Streaming operator.
#include <EEPROM.h> // Include this library for using the EEPROM flas storage on the huzzah.
#include <Configuration.h> // This library contains the code for loading plant pot configuration.
#include <Communication.h> // This library contains the code for communication between the pot and broker.
#include <PlantCare.h> // This library contains the code for taking care of the plant.
#include <LedController.h> // This library contains the code for taking care of the plant.

Configuration configuration;
Communication communication( &configuration );
PlantCare plantCare( &communication );
LedController ledController;

void setup()
{
    Serial.begin(115200);
    configuration.setup();
    communication.setup();
    ledController.setup();
}

void loop()
{
    int waterLevel = plantCare.checkWaterReservoir();
    ledController.setColorBasedOnWaterLevel(waterLevel);
    plantCare.takeCareOfPlant();
}

