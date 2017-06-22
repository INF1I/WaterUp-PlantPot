/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 17-06-2017 21:46
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "PlantCare.h"

/**
 * This function initiates the plant care library. It sets up the
 * I/O pins that are connected to the sensors and water pump and
 * initiates the time keeper variables.
 */
PlantCare::PlantCare( Communication *potCommunication )
{
    long whatTimeIsIt = millis();
    this->waterPumpState = LOW;
    this->communication = potCommunication;
    this->configuration = communication->getConfiguration();
    this->currentWarning = this->configuration->WarningType::NO_ERROR;
    this->publishReservoirWarningThreshold= this->configuration->getMqttSettings()->publishReservoirWarningThreshold;

    this->lastPublishStatisticsTime = whatTimeIsIt;
    this->lastPublishWarningTime = whatTimeIsIt;
    this->lastPingTime = whatTimeIsIt;
    this->lastMeasurementTime = whatTimeIsIt;
    this->lastGivingWaterTime = whatTimeIsIt;

    this->publishStatisticInterval = configuration->getMqttSettings()->statisticPublishInterval;
    this->republishWarningInterval = configuration->getMqttSettings()->resendWarningInterval;
    this->pingInterval = configuration->getMqttSettings()->pingBrokerInterval;
    this->takeMeasurementInterval = configuration->getPlantCareSettings()->takeMeasurementInterval;
    this->sleepAfterGivingWaterTime = configuration->getPlantCareSettings()->sleepAfterGivingWater;
    this->groundMoistureOptimal = configuration->getPlantCareSettings()->groundMoistureOptimal;

    this->red = configuration->getLedSettings()->red;
    this->green = configuration->getLedSettings()->green;
    this->blue = configuration->getLedSettings()->blue;

    pinMode( IO_PIN_SONAR_TRIGGER, OUTPUT );
    pinMode( IO_PIN_SONAR_ECHO, OUTPUT );
    pinMode( IO_PIN_SOIL_MOISTURE, INPUT );
    pinMode( IO_PIN_WATER_PUMP, OUTPUT );
    digitalWrite( IO_PIN_WATER_PUMP, LOW ); // Make sure we don't give the drown the plant.
}

/**
 * This is the main function of the project. It will take care of the
 * plant and control everything.
 */
void PlantCare::takeCareOfPlant()
{
    this->currentTime = millis();
    this->communication->connect(); // Are we still connected?
    this->publishPotStatistic();
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
    if( this->lastMeasurementTime - this->currentTime == this->takeMeasurementInterval && this->lastGivingWaterTime - currentTime > sleepAfterGivingWaterTime)
    {

        int currentGroundMoisture = checkMoistureLevel();

        if( currentGroundMoisture < this->groundMoistureOptimal )
        {
            activateWaterPump();
            delay( WATER_PUMP_DEFAULT_TIME );
            deactivateWaterPump();
        }

    }
}

void PlantCare::activateWaterPump()
{
    Serial << F("[debug] Activating the water pump.") << endl;
    digitalWrite(IO_PIN_WATER_PUMP, HIGH );
}

void PlantCare::deactivateWaterPump()
{
    Serial << F("[debug] Deactivating the water pump.") << endl;
    digitalWrite(IO_PIN_WATER_PUMP, LOW );
}

void PlantCare::publishPotStatistic()
{
    if( this->lastPublishStatisticsTime - this->currentTime == this->publishStatisticInterval)
    {
        int waterLevel = this->checkWaterReservoir();

        if( waterLevel < this->publishReservoirWarningThreshold )
        {
            this->currentWarning = waterLevel > 5 ? this->configuration->LOW_RESERVOIR : this->configuration->EMPTY_RESERVOIR;
            this->publishPotWarning( this->currentWarning );
        }

        this->communication->publishStatistic( this->checkMoistureLevel(), waterLevel );
    }
}

void PlantCare::publishPotWarning( uint8_t warningType )
{
    if( this->lastPublishWarningTime - this->currentTime == this->republishWarningInterval && this->currentWarning )
    {
        this->communication->publishWarning(warningType);
    }
}