/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 17-06-2017 21:46
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "PlantCare.h"

/**
 * This function initiates the plant care library. It sets up the
 * I/O pins that are connected to the sensors and water pump.
 */
PlantCare::PlantCare( Communication *potCommunication )
{
    this->waterPumpState = LOW;
    this->communication = potCommunication;
    this->configuration = communication->getConfiguration();

    pinMode( IO_PIN_SONAR_TRIGGER, OUTPUT );
    pinMode( IO_PIN_SONAR_ECHO, OUTPUT );
    pinMode( IO_PIN_SOIL_MOISTURE, INPUT );
    pinMode( IO_PIN_WATER_PUMP, OUTPUT );
    digitalWrite( IO_PIN_WATER_PUMP, LOW );
}

/**
 * This is the main function of the project. It will take care of the
 * plant and control everything.
 */
void PlantCare::takeCareOfPlant()
{

}

/**
 * This function will use the ultra sonic sensor to measure percentage
 * of water left in the reservoir.
 * @return int - The percentage of water left in the reservoir.
 */
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
    long waterContent = waterDistance > 30 ? BOTTOM_CONTENT(waterDistance) : RESERVOIR_BOTTOM_SIZE + TOP_CONTENT(waterDistance-10);
    return (int)(RESERVOIR_SIZE/100)*waterContent;
}

/**
 * This function will use the ground moisture sensor to measure the resistance
 * of the soil. If its wet the resistance is les so we know how wet the ground is.
 * @return int - The percentage resistance the soil has.
 */
int PlantCare::checkMoistureLevel()
{
    return (int)(1024/analogRead( IO_PIN_SOIL_MOISTURE ))*100;
}

void PlantCare::giveWater()
{
    digitalWrite(IO_PIN_WATER_PUMP, HIGH );
    delay( WATER_PUMP_DEFAULT_TIME );
}

void PlantCare::giveWater( unsigned long duration )
{
    Serial << F("[debug] Activating the water pump for: ") << duration << F("seconds") << endl;
    //activateWaterPump();
    delay(duration);
    //deactivateWaterPump();
}

void PlantCare::publishPotStatistic()
{
    communication->publishStatistic( checkMoistureLevel(), checkWaterReservoir() );
}

void PlantCare::publishPotWarning( uint8_t warningType )
{
    communication->publishWarning( warningType );
}