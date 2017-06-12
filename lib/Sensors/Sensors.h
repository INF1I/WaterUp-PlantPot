/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_SENSORS_SENSORS_H
#define WATERUP_SENSORS_SENSORS_H

#include <Arduino.h> // Include this library so we can use the arduino system functions and variables.
#include <Streaming.h> // Include this library for using the << Streaming operator.

#define potHeight 40 // Height what the water can reach
#define waterReservoirSurfaceSize 500 // The cm2 surface of the water reservoir.
#define waterReservoirSize 20000 // The cm3 content of the water reservoir.

#define trigPin 13 // The pin connected trigger port of the ultra sonar sensor.
#define echoPin 12 // The pin connected to the echo port of the ultra sonar sensor.
#define soilSensePin A0 // The pin connected to the analog read of the soil moisture sensor.
#define waterPumpPin 16 // The pin connected to the transistor base for switching the water pump.

class Sensors;

class Sensors
{
private:
    unsigned long previousWaterPumpMillis;

public:
    /**
     * Initiate all sensors.
     */
    void setup();

    /**
     * Get the distance in centimeters to the water in the water rersorvoir.
     * @return long The distance to the water.
     */
    long getDistance();

    /**
     * Get the soil resistance mesured by the soil moisture sensor.
     * @return int The resistance of the soil.
     */
    int getMoistureLevel();

    /**
     * Calculate the percentage of water left in the water resorvoir.
     * @return waterLevel The percentage of water left in the resorvoir.
     */
    int calcWaterLevel();

    /**
     * This function will activate the water pump for an specified duration.
     */
    void activateWaterPump(unsigned long duration );

    /**
     * This function will start the water pump.
     */
    void activateWaterPump();

    /**
     * This function will stop the water pump.
     */
    void deactivateWaterPump();
};

#endif // WATERUP_SENSORS_SENSORS_H