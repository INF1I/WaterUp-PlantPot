/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Created: 19-06-2017 17:51
 * Licence: GPLv3 - General Public Licence version 3
 */
#include <Arduino.h> // Include this library for using basic system functions and variables.
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
//    configuration.setup();
//    communication.setup();
//    configuration.getPlantCareSettings()->takeMeasurementInterval;
    ledController.setup();
}

void loop()
{
    ledController.setColor(0,200,200);
//    plantCare.takeCareOfPlant();
}