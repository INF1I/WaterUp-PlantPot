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
    pinMode( trigPin, OUTPUT );
    pinMode( echoPin, OUTPUT );
    pinMode( soilSensePin, INPUT );
    pinMode( waterPumpPin, OUTPUT );
    digitalWrite( waterPumpPin, LOW );
}

void PlantCare::takeCareOfPlant()
{

}

int PlantCare::checkWaterReservoir()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long Duration = pulseIn(echoPin, HIGH); //Listening and waiting for wave
    delay(10);
    long distanceToWaterInCM = (Duration * 0.034 / 2);//Convert echo time measurement to centimeters.
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