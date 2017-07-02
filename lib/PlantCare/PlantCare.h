/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroesen <alwin.kroesen@student.stenden.com>
 * Created: 17-06-2017 21:46
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_PLANTPOT_PLANTCARE_H
#define WATERUP_PLANTPOT_PLANTCARE_H

#include <Arduino.h> // Include this library for using basic system functions and variables.
#include <Streaming.h> // Include this library for using the << Streaming operator.
#include <Configuration.h> // This library contains the code for loading plant pot configuration.
#include <Communication.h> // This library contains the code for communication between the pot and broker.
#include <LedController.h> // This library contains the code for taking care of the plant.

#define RESERVOIR_TOP_HEIGHT 30 // The height of the top of the reservoir.
#define RESERVOIR_TOP_1CM3 500// 1cm = 500cm³
#define RESERVOIR_TOP_SIZE 15000 // The cubic centimeter content of top the water reservoir.

#define RESERVOIR_BOTTOM_HEIGHT 10 // The height of the bottom of the reservoir.
#define RESERVOIR_BOTTOM_1CM3 900 // 1cm = 900cm³
#define RESERVOIR_BOTTOM_SIZE 9000 // The cubic centimeter content of bottom the water reservoir.
#define RESERVOIR_SIZE 24000 // The total cubic centimeter content of the reservoir.

#define BOTTOM_CONTENT( cm ) (RESERVOIR_BOTTOM_HEIGHT-cm)*RESERVOIR_BOTTOM_1CM3
#define TOP_CONTENT( cm ) (RESERVOIR_TOP_HEIGHT-cm)*RESERVOIR_TOP_1CM3

#define potLength 30
#define potWidth 30
#define potHeight 40 // Height what the water can reach
#define innerPotLength 20
#define innerPotWidth 20

#define IO_PIN_SONAR_TRIGGER 13 // The pin connected trigger port of the ultra sonar sensor.
#define IO_PIN_SONAR_ECHO 12 // The pin connected to the echo port of the ultra sonar sensor.
#define IO_PIN_SOIL_MOISTURE A0 // The pin connected to the analog read of the soil moisture sensor.
#define IO_PIN_WATER_PUMP 16 // The pin connected to the transistor base for switching the water pump.

#define WATER_PUMP_DEFAULT_TIME 5000 // The default time to activate the water pump.

class Communication; // Forward declare the communication library.
class Configuration; //  Forward declare the configuration library.
class PlantCare; // Forward declare the plant care library.

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

    /**
     * This function will use the ultra sonic sensor to measure percentage
     * of water left in the reservoir.
     * @return int - The percentage of water left in the reservoir.
     */
    int checkWaterReservoir();

private:
    bool waterPumpState; // The current state of the water pump, either on or off.
    Configuration* configuration; // An configuration instance containing mqtt, led and plant care configuration.
    Communication* communication; // An communication instance for communication between the pot and mqtt broker.

    uint32_t currentTime; // The current milliseconds since the last reset.
    uint32_t lastPublishStatisticsTime; // The time in milliseconds we published statistics to the broker.
    uint32_t lastPublishWarningTime; // The last time in milliseconds we published an warning to the broker.
    uint32_t lastPingTime; // The last time in milliseconds we pinged to the broker.
    uint32_t lastMeasurementTime; // The last time in milliseconds we took an measurement.
    uint32_t lastGivingWaterTime; // The last time in milliseconds we gave water.

    uint8_t currentWarning; // The current warning code.

    // Led settings
    uint8_t red; // The luminosity strength of the red led in the reservoir.
    uint8_t green; // The luminosity strength of the green led in the reservoir.
    uint8_t blue; // The luminosity strength of the blue led in the reservoir.

    // MQTT settings
    uint32_t publishStatisticInterval; // The interval in milliseconds we publish statistics to the broker.
    uint32_t republishWarningInterval; // The interval in milliseconds to republish warning messages to the broker.
    uint32_t pingInterval; // The interval in milliseconds to ping to the broker.
    uint8_t publishReservoirWarningThreshold; // The time to wait after publishing warning messages.

    // Plant care settings.
    uint32_t takeMeasurementInterval; // The interval in milliseconds to take pot measurements.
    uint32_t sleepAfterGivingWaterTime; // The time to wait before giving water to the pant again.
    uint8_t groundMoistureOptimal; // The optimal ground moisture level.
    uint8_t containsPlant; // Boolean to check if the pot contains an plant.

    /**
     * This function will use the ground moisture sensor to measure the resistance
     * of the soil. If its wet the resistance is les so we know how wet the ground is.
     * @return int - The percentage resistance the soil has.
     */
    int checkMoistureLevel();

    /**
     * This function will take care of giving the plant water. It will give water based on the
     * configured interval and sleep time.
     */
    void giveWater();

    /**
     * This function will switch the water pump state if it was on it will be switched off
     * and vise versa.
     */
    //void switchWaterPump();

    /**
     * This function will take care of publishing pot statistics to the broker based on
     * the configured interval and previous published message.
     */
    void publishPotStatistic();

    /**
     * This function will take care of publishing pot warnings to the broker based on
     * the configured republish intervals and previous published message.
     *
     * @param warningType   The type of warning to publish like an empty or near empty reservoir.
     */
    void publishPotWarning( uint8_t warningType );

    /**
     * This function will measure the distance from the top of the water reservoir to the
     * water level using the ultra sonic sensor.
     *
     * @return  The distance to the water surface in centimeters.
     */
    long getDistance();

    /**
     * This function will switch the water pump on so the plant receives water.
     */
    void activateWaterPump();

    /**
     * This function will switch the water pump off so the plant stops receiving water.
     */
    void deactivateWaterPump();
};

#endif //WATERUP_PLANTPOT_PLANTCARE_H
