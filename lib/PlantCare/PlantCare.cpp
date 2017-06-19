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
    long waterDistance = (Duration * 0.034 / 2);//Convert echo time measurement to centimeters.
    long waterContent = waterDistance > 30 ? BOTTOM_CONTENT(distanceToWaterInCM) : RESERVOIR_BOTTOM_SIZE + TOP_CONTENT(waterDistance-10);
    return percentageFull = (int)(RESERVOIR_SIZE/100)*waterContent;
}

int PlantCare::checkMoistureLevel()
{
    return (int)(1024/analogRead(soilSensePin))*100;
}

void PlantCare::giveWater()
{
    digitalWrite(waterPumpPin, HIGH );
    delay( WATER_PUMP_DEFAULT_TIME );
}

void PlantCare::giveWater( unsigned long duration )
{
    Serial << F("[debug] Activating the water pump for: ") << duration << F("seconds") << endl;
    activateWaterPump();
    delay(duration);
    deactivateWaterPump();
}

void PlantCare::publishPotStatistic()
{
    communication.publishStatistic( checkMoistureLevel(), checkWaterReservoir() );
}

void PlantCare::publishPotWarning( WarningType warningType )
{
    communication.publishWarning( warningType );
}