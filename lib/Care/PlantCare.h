/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 17-06-2017 21:46
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_PLANTPOT_PLANTCARE_H
#define WATERUP_PLANTPOT_PLANTCARE_H

#include <Arduino.h> // Include this library so we can use the arduino system functions and variables.
#include <Streaming.h> // Include this library for using the << Streaming operator.

#define RESERVOIR_HEIGHT 30 // Height what the water can reach
#define RESERVOIR_TOP_SURFACE 500 // The cm2 surface of the water reservoir.
#define waterReservoirSize 20000 // The cm3 content of the water reservoir.

// waterReservoirTop = 30*( (30*30) - (20*20) )
// waterReservoirBottom = 10 * ( 30 * 30 )
// waterReservoirSize = waterReservoirTop+waterReservoirBottom

#define trigPin 13 // The pin connected trigger port of the ultra sonar sensor.
#define echoPin 12 // The pin connected to the echo port of the ultra sonar sensor.
#define soilSensePin A0 // The pin connected to the analog read of the soil moisture sensor.
#define waterPumpPin 16 // The pin connected to the transistor base for switching the water pump.

class PlantCare;

enum WarningType
{
    LOW_RESORVOIR = 1,
    EMPTY_RESORVOIR = 2,
    LOW_MOISTURE_LEVEL = 3,
    HIGH_MOISTURE_LEVEL = 4,
    UNKNOWN_ERROR = 5
};

class PlantCare
{
public:
    PlantCare( Communication * communication );
    void setup();
    void takeCareOfPlant();

private:
    bool waterPumpState;
    Communication communication;
    int checkWaterReservoir();
    int checkMoistureLevel();
    void giveWater();
    void giveWater( unsigned long duration );
    void switchWaterPump();
    void publishPotStatistic();
    void publishPotWarning( WarningType warningType );
};


#endif //WATERUP_PLANTPOT_PLANTCARE_H
