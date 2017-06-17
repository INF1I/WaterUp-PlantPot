/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 17-06-2017 21:46
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "PlantCare.h"

PlantCare::PlantCare(Communication *communication)
{
    this->communication = communication;
}

/**
 * Initiate the plant care library, this will setup the I/O pin modes of the connected sensors and water pump.
 */
void PlantCare::setup()
{
    pinMode( IO_PIN_SONAR_TRIGGER, OUTPUT );
    pinMode( IO_PIN_SONAR_ECHO, OUTPUT );
    pinMode( IO_PIN_SOIL_MOISTURE, INPUT );
    pinMode( IO_PIN_WATER_PUMP, OUTPUT );
    digitalWrite( IO_PIN_WATER_PUMP, LOW );
}

void PlantCare::takeCareOfPlant()
{

}

int PlantCare::checkWaterReservoir()
{
    digitalWrite( IO_PIN_SONAR_TRIGGER, LOW);
    delayMicroseconds(2);
    digitalWrite(IO_PIN_SONAR_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(IO_PIN_SONAR_TRIGGER, LOW);

    long Duration = pulseIn(IO_PIN_SONAR_ECHO, HIGH); //Listening and waiting for wave
    delay(10);
    long distanceToWaterInCM = (Duration * 0.034 / 2);//Convert echo time measurement to centimeters.
    long waterContent = distanceToWaterInCM < 30 ? (RESERVOIR_BOTTOM_HEIGHT-distanceToWaterInCM)*RESERVOIR_BOTTOM_1CM3 : (RESERVOIR_TOP_HEIGHT-distanceToWaterInCM)*RESERVOIR_TOP_1CM3+
    float waterContent = waterReservoirSurfaceSize * ( potHeight - distance );
    int waterLevel = (int) (waterContent / waterReservoirSize * 100);
    waterLevel = waterLevel < 0 ? 0: waterLevel;
}

int PlantCare::checkMoistureLevel()
{

}

void PlantCare::giveWater()
{

}

void PlantCare::giveWater( unsigned long duration )
{

}

void PlantCare::publishPotStatistic()
{

}

void PlantCare::publishPotWarning( WarningType warningType )
{

}