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
    this->lastPublishStatisticsTime = whatTimeIsIt;
    this->lastPublishWarningTime = whatTimeIsIt;
    this->lastPingTime = whatTimeIsIt;
    this->lastMeasurementTime = whatTimeIsIt;
    this->lastGivingWaterTime = whatTimeIsIt;

    this->waterPumpState = LOW; // Set the current state of the water pump to LOW so its off when we start.
    this->communication = potCommunication; // Set the communication instance for communication between the pot and mqtt broker.
    this->configuration = communication->getConfiguration(); // Set tge configuration instance containing mqtt, led and plant care configuration.
    this->currentWarning = this->configuration->WarningType::NO_ERROR;

    // Led settings
    this->red = configuration->getLedSettings()->red;
    this->green = configuration->getLedSettings()->green;
    this->blue = configuration->getLedSettings()->blue;

    // MQTT settings
    this->publishStatisticInterval = configuration->getMqttSettings()->statisticPublishInterval;
    this->republishWarningInterval = configuration->getMqttSettings()->resendWarningInterval;
    this->pingInterval = configuration->getMqttSettings()->pingBrokerInterval;
    this->publishReservoirWarningThreshold = this->configuration->getMqttSettings()->publishReservoirWarningThreshold;

    // Plant care settings
    this->takeMeasurementInterval = configuration->getPlantCareSettings()->takeMeasurementInterval;
    this->sleepAfterGivingWaterTime = configuration->getPlantCareSettings()->sleepAfterGivingWater;
    this->groundMoistureOptimal = configuration->getPlantCareSettings()->groundMoistureOptimal;
    this->containsPlant = configuration->getPlantCareSettings()->containsPlant;

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
 * Check if there is connection to the mqtt broker. If not attempt to reconnect to the broker.
 * Start publishing messages and listening for messages from the broker. And take care of the plant.
 */
void PlantCare::takeCareOfPlant()
{
    this->currentTime = millis();
    this->communication->connect(); // Are we still connected?
    this->communication->listenForConfiguration();

    if( this->containsPlant == 1 )
    {
        this->publishPotStatistic();
        this->giveWater();
    }
}

/**
 * This function will use the ultra sonic sensor to measure percentage
 * of water left in the reservoir.
 *
 * @return int - The percentage of water left in the reservoir.
 */
int PlantCare::checkWaterReservoir()
{
    long startTime = millis();
    POT_DEBUG_PRINTLN( F("[debug] - Checking the water reservoir") )

    // Clear the trigger pin.
    digitalWrite(IO_PIN_SONAR_TRIGGER, LOW);
    delayMicroseconds(2);

    // Send short pulse to the trigger pin
    digitalWrite(IO_PIN_SONAR_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(IO_PIN_SONAR_TRIGGER, LOW);

    // Measure the time it took for the sound wave to return to the sensor.
    long responseTime = pulseIn(IO_PIN_SONAR_ECHO, HIGH); //Listening and waiting for wave
    delay(10);

    // Convert response time in microseconds to distance in centimeters.
    long cmDistanceToWaterSurface  = responseTime * SOUND_SPEED_CM_PER_MICRO_SECOND / 2;
    POT_DEBUG_PRINTLN( F("[debug] - The distance from the ultrasonic sensor to the water surface is: ") APPEND cmDistanceToWaterSurface )

    // Calculate how much water is left in the reservoir.
    uint16_t waterLeftInReservoir = RESERVOIR_CONTENT_CM_3 - (RESERVOIR_1_CM_CONTENT_CM_3 * cmDistanceToWaterSurface );

    // Convert it to an percentage.
    return waterLeftInReservoir / (RESERVOIR_CONTENT_CM_3/100);
}

/**
 * This function will use the ground moisture sensor to measure the resistance
 * of the soil. If its wet the resistance is les so we know how wet the ground is.
 * @return int - The percentage resistance the soil has.
 */
int PlantCare::checkMoistureLevel()
{
    uint16_t soilResistance = analogRead(IO_PIN_SOIL_MOISTURE);
    uint8_t percentageOfSoilMoisture = soilResistance / (1024/100);

    POT_DEBUG_PRINTLN( F("[debug] - Checking the soil moisture level") NEW_LINE
    F("[debug] - Measured ") APPEND soilResistance F( "/1024 so the percentage is: " ) APPEND percentageOfSoilMoisture)

    return percentageOfSoilMoisture;
}

/**
 *Take care of giving the plant water. Give water based on the interval configured and wait for an certain
 * time after giving water so the water has time to spread through the soil.
 */
void PlantCare::giveWater()
{
    if( this->currentTime - this->lastMeasurementTime > this->takeMeasurementInterval && this->currentTime - this->lastGivingWaterTime > sleepAfterGivingWaterTime)
    {
        Serial << F("[debug] - Giving water to the plant.") << endl;
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
 * Write an voltage on the water pump pin so the transistor will allow the 12v current
 * to flow through the water pump.
 */
void PlantCare::activateWaterPump()
{
    Serial << F("[debug] Activating the water pump.") << endl;
    digitalWrite(IO_PIN_WATER_PUMP, HIGH );
}

/**
 * Switch the transistor off so the power to the water pump gets cut.
 */
void PlantCare::deactivateWaterPump()
{
    Serial << F("[debug] Deactivating the water pump.") << endl;
    digitalWrite(IO_PIN_WATER_PUMP, LOW );
}

/**
 * Take care of publishing pot statistics to the broker based on the configured
 * interval and previous tine an message was published.
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
 * Take care of publishing pot warnings to the broker based on the configured republish
 * intervals and previously send warning message.
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