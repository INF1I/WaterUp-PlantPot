/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Created: 19-06-2017 17:51
 * Licence: GPLv3 - General Public Licence version 3
 */
#include <Arduino.h> // Include this library for using basic system functions and variables.
#include <Streaming.h> // Include this library for using the << Streaming operator.
#include <Configuration.h> // This library contains the code for loading plant pot configuration.
#include <Communication.h> // This library contains the code for communication between the pot and broker.
#include <PlantCare.h> // This library contains the code for taking care of the plant.
#include <LedController.h> // This library contains the code for taking care of the plant.

/**
 * This configuration instance will handle receiving and persisting pot configuration
 * from and to the eeprom storage.
 */
Configuration configuration;

/**
 * This communication instance will handle the wifi connection and all communication
 * between the pot and MQTT broker.
 */
Communication communication( &configuration );

/**
 * This plant care instance will take care of the plant and manage the all processes
 * associated with the plant pot, like giving water, publishing statistics and listening
 * for net pot configuration.
 */
PlantCare plantCare( &communication );

/**
 * This led controller instance will control the led lightning in the water reservoir. It
 * will handle the the luminosity and colour of the led's.
 */
LedController ledController;

/**
 * This is the standard entry point of the code it will initiate the libraries and start
 * serial communication for debugging purposes. It will get executed after every poser circle.
 */
void setup()
{
    communication.setup();
    ledController.setup();
}

/**
 * This is the standard process of the plant pot. It iterate over this function as long as
 * the pot is powered on. This function will call the takeCareOfPlant() function which will
 * start the pot's main program.
 */
void loop()
{
    int waterLevel = plantCare.checkWaterReservoir();
    ledController.setColorBasedOnWaterLevel(waterLevel);
    plantCare.takeCareOfPlant();
}

