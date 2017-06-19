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

#define RESERVOIR_TOP_HEIGHT 30 // The height of the top of the reservoir.
#define RESERVOIR_TOP_1CM3 500// 1cm = 500cm³
#define RESERVOIR_TOP_SIZE 15000 // The cubic centimeter content of top the water reservoir.

#define RESERVOIR_BOTTOM_HEIGHT 10 // The height of the bottom of the reservoir.
#define RESERVOIR_BOTTOM_1CM3 900 // 1cm = 900cm³
#define RESERVOIR_BOTTOM_SIZE 9000 // The cubic centimeter content of bottom the water reservoir.
#define RESERVOIR_SIZE 24000 // The total cubic centimeter content of the reservoir.

#define BOTTOM_CONTENT( cm ) (RESERVOIR_BOTTOM_HEIGHT-cm)*RESERVOIR_BOTTOM_1CM3
#define TOP_CONTENT( cm ) (RESERVOIR_TOP_HEIGHT-cm)*RESERVOIR_TOP_1CM3

#define IO_PIN_SONAR_TRIGGER D13 // The pin connected trigger port of the ultra sonar sensor.
#define IO_PIN_SONAR_ECHO D12 // The pin connected to the echo port of the ultra sonar sensor.
#define IO_PIN_SOIL_MOISTURE A0 // The pin connected to the analog read of the soil moisture sensor.
#define IO_PIN_WATER_PUMP 16 // The pin connected to the transistor base for switching the water pump.

#define WATER_PUMP_DEFAULT_TIME 5000 // The default time to activate the water pump.

class PlantCare;

enum class WarningType
{
    LowReservoir = 1,
    EmptyReservoir = 2,
    LowMoistureLevel = 3,
    HighMoistureLevel = 4,
    UnknownError = 5
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
