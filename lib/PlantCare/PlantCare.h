/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 17-06-2017 21:46
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_PLANTPOT_PLANTCARE_H
#define WATERUP_PLANTPOT_PLANTCARE_H

#include <Arduino.h> // Include this library for using basic system functions and variables.
#include <ESP8266WiFi.h> // Include this library for working with the ESP8266 chip.
#include <WiFiManager.h> // Include this library for dynamically setting up the WiFi connection.
#include <Adafruit_MQTT.h> // Include this library for securely connecting to the internet using WiFi.
#include <Adafruit_MQTT_Client.h> // Include this library for MQTT communication.
#include <FS.h> // Include this library for access to the ESP8266's file system.
#include <Streaming.h> // Include this library for using the << Streaming operator.
#include <EEPROM.h> // Include this library for using the EEPROM flas storage on the huzzah.
#include <Configuration.h> // This library contains the code for loading plant pot configuration.
#include <Communication.h> // This library contains the code for communication between the pot and broker.
//#include <PlantCare.h> // This library contains the code for taking care of the plant.

#define RESERVOIR_TOP_HEIGHT 30 // The height of the top of the reservoir.
#define RESERVOIR_TOP_1CM3 500// 1cm = 500cm³
#define RESERVOIR_TOP_SIZE 15000 // The cubic centimeter content of top the water reservoir.

#define RESERVOIR_BOTTOM_HEIGHT 10 // The height of the bottom of the reservoir.
#define RESERVOIR_BOTTOM_1CM3 900 // 1cm = 900cm³
#define RESERVOIR_BOTTOM_SIZE 9000 // The cubic centimeter content of bottom the water reservoir.
#define RESERVOIR_SIZE 24000 // The total cubic centimeter content of the reservoir.

#define BOTTOM_CONTENT( cm ) (RESERVOIR_BOTTOM_HEIGHT-cm)*RESERVOIR_BOTTOM_1CM3
#define TOP_CONTENT( cm ) (RESERVOIR_TOP_HEIGHT-cm)*RESERVOIR_TOP_1CM3

#define IO_PIN_SONAR_TRIGGER 13 // The pin connected trigger port of the ultra sonar sensor.
#define IO_PIN_SONAR_ECHO 12 // The pin connected to the echo port of the ultra sonar sensor.
#define IO_PIN_SOIL_MOISTURE A0 // The pin connected to the analog read of the soil moisture sensor.
#define IO_PIN_WATER_PUMP 16 // The pin connected to the transistor base for switching the water pump.

#define WATER_PUMP_DEFAULT_TIME 5000 // The default time to activate the water pump.

class Communication;
class Configuration;
class PlantCare;

class PlantCare
{
public:
    /**
     * This function initiates the plant care library. It sets up the
     * I/O pins that are connected to the sensors and water pump.
     */
    PlantCare( Communication* potCommunication );

    /**
     * This is the main function of the project. It will take care of the
     * plant and control everything.
     */
    void takeCareOfPlant();

private:
    bool waterPumpState; // The current state of the water pump, either on or off.
    Configuration* configuration; // An configuration instance containing mqtt, led and plant care configuration.
    Communication* communication; // An communication instance for communication between the pot and mqtt broker.

    uint16_t lastPublishStatisticsTime;
    uint16_t lastPublishWarningTime;
    uint16_t lastPingTime;
    uint16_t lastMeasurementTime;
    uint16_t lastGivingWaterTime;

    uint16_t publishStatisticInterval;
    uint16_t republishWarningInterval;
    uint16_t pingInterval;
    uint16_t takeMeasurementInterval;
    uint16_t sleepAfterGivingWaterTime;
    uint8_t groundMoistureOptimal;

    uint8_t red;
    uint8_t green;
    uint8_t blue;

    /**
     * This function will use the ultra sonic sensor to measure percentage
     * of water left in the reservoir.
     * @return int - The percentage of water left in the reservoir.
     */
    int checkWaterReservoir();

    /**
     * This function will use the ground moisture sensor to measure the resistance
     * of the soil. If its wet the resistance is les so we know how wet the ground is.
     * @return int - The percentage resistance the soil has.
     */
    int checkMoistureLevel();

    void giveWater();
    void giveWater( unsigned long duration );

    void switchWaterPump();
    void activateWaterPump();
    void deactivateWaterPump();

    void publishPotStatistic();
    void publishPotWarning( uint8_t warningType );
};

#endif //WATERUP_PLANTPOT_PLANTCARE_H
