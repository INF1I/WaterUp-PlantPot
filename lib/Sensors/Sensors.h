#ifndef SENSORS_H
#define SENSORS_H

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
};

#endif