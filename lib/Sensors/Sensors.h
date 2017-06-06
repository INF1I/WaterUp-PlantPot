/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef SENSORS_H
#define SENSORS_H

#define potHeight 40 // Height what the water can reach
#define waterReservoirSurfaceSize 500 // The cm2 surface of the water reservoir.
#define waterReservoirSize 20000 // The cm3 content of the water reservoir.

#include <Arduino.h> // Include this library so we can use the arduino system functions and variables.

class Sensors;

class Sensors
{
private:
    /**
     * Initiate the I/O pin's connected to the ultrasonic sensor.
     */
    void setupUltraSonic();

    /**
     * Initiate the I/O pin's connected to the soil moister sensor.
     */
    void setupMoistureDetector();

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
};

#endif