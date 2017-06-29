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
    /**
     * The assignment statements below will set the basic pot configuration from the config library
     * and save it to this object attributes.
     */
    long whatTimeIsIt = millis(); // The current milliseconds since the last reset.
    this->waterPumpState = LOW; // Set the current state of the water pump to LOW so its off when we start.
    this->communication = potCommunication; // Set the communication instance for communication between the pot and mqtt broker.
    this->configuration = communication->getConfiguration(); // Set tge configuration instance containing mqtt, led and plant care configuration.
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

    /**
     * The pim mode function calls below will setup the I/O pin modes to either input or output.
     */
    pinMode( IO_PIN_SONAR_TRIGGER, OUTPUT );
    pinMode( IO_PIN_SONAR_ECHO, INPUT );
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
    this->giveWater();

}

/**
 * This function will use the ultra sonic sensor to measure percentage
 * of water left in the reservoir.
 * @return int - The percentage of water left in the reservoir.
 */
int PlantCare::checkWaterReservoir()
{
    float distance = getDistance();
    long surface = (potLength * potWidth) - (innerPotLength * innerPotWidth);
    float content = surface * potHeight;

    float waterContent = surface * ( potHeight - distance );
    int waterLevel = (int) (waterContent / content * 100);
    if(waterLevel < 0) waterLevel = 0;
    return (int)waterLevel;
}


/**
 * Get the distance in centimeters to the water in the water rersorvoir.
 * @return long The distance to the water.
 */
long PlantCare::getDistance()
{
    digitalWrite(IO_PIN_SONAR_TRIGGER, LOW);
    delayMicroseconds(2);
    digitalWrite(IO_PIN_SONAR_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(IO_PIN_SONAR_TRIGGER, LOW);

    long Duration = pulseIn(IO_PIN_SONAR_ECHO, HIGH); //Listening and waiting for wave
    delay(10);
    return (Duration * 0.034 / 2);//Converting the reported number to CM
}

/**
 * Get the soil resistance mesured by the soil moisture sensor.
 * @return int The resistance of the soil.
 */
int PlantCare::getMoistureLevel()
{
    int v = analogRead(IO_PIN_SOIL_MOISTURE);
    return v;
}

/**
 * This function will use the ground moisture sensor to measure the resistance
 * of the soil. If its wet the resistance is les so we know how wet the ground is.
 * @return int - The percentage resistance the soil has.
 */
int PlantCare::checkMoistureLevel()
{
    return (int)this->getMoistureLevel();
    //return (int)(1024/analogRead( IO_PIN_SOIL_MOISTURE ) )*100;
}

/**
 * This function will take care of giving the plant water. It will give water based on the
 * configured interval and sleep time.
 */
void PlantCare::giveWater()
{
    if( this->currentTime - this->lastMeasurementTime > this->takeMeasurementInterval && this->currentTime - this->lastGivingWaterTime > sleepAfterGivingWaterTime)
    {
        Serial << F("[debug] - ") << endl;
        this->lastMeasurementTime = currentTime;

        int currentGroundMoisture = checkMoistureLevel();

        if( currentGroundMoisture < this->groundMoistureOptimal )
        {
            activateWaterPump();
            delay( WATER_PUMP_DEFAULT_TIME );
            deactivateWaterPump();
            this->lastGivingWaterTime = currentTime+WATER_PUMP_DEFAULT_TIME;
        }
    }
}

/**
 * This function will switch the water pump on so the plant receives water.
 */
void PlantCare::activateWaterPump()
{
    Serial << F("[debug] Activating the water pump.") << endl;
    digitalWrite(IO_PIN_WATER_PUMP, HIGH );
}

/**
 * This function will switch the water pump off so the plant stops receiving water.
 */
void PlantCare::deactivateWaterPump()
{
    Serial << F("[debug] Deactivating the water pump.") << endl;
    digitalWrite(IO_PIN_WATER_PUMP, LOW );
}

/**
 * This function will take care of publishing pot statistics to the broker based on
 * the configured interval and previous published message.
 */
void PlantCare::publishPotStatistic()
{
    if( this->currentTime - this->lastPublishStatisticsTime > this->publishStatisticInterval )
    {
        this->lastPublishStatisticsTime = this->currentTime;
        int waterLevel = this->checkWaterReservoir();
        if(waterLevel == 0) waterLevel = 1;
        if( waterLevel < this->publishReservoirWarningThreshold )
        {
            this->currentWarning = waterLevel > 5 ? this->configuration->LOW_RESERVOIR : this->configuration->EMPTY_RESERVOIR;
            this->publishPotWarning( this->currentWarning );
        }

        this->communication->publishStatistic( this->checkMoistureLevel(), waterLevel );
    }
}

/**
 * This function will take care of publishing pot warnings to the broker based on
 * the configured republish intervals and previous published message.
 *
 * @param warningType   The type of warning to publish like an empty or near empty reservoir.
 */
void PlantCare::publishPotWarning( uint8_t warningType )
{
    if( this->currentTime - this->lastPublishWarningTime > this->republishWarningInterval && this->currentWarning )
    {
        this->lastPublishWarningTime = this->currentTime;
        this->communication->publishWarning(warningType);
    }
}