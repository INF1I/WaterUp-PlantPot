/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef SENSORS_H
#define SENSORS_H

#define potLength 30
#define potWidth 30
#define potHeight 40 // Height what the water can reach
#define innerPotLength 20
#define innerPotWidth 20
#include <Arduino.h>


class Sensors;

class Sensors{
    private:
        void setupUltraSonic();
        void setupMoistureDetector();
    public:
        void setup();
        long getDistance();
        int getMoistureLevel();
        int calcWaterLevel();
};

#endif