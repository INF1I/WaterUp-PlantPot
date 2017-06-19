/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 19-06-2017 17:51
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_PLANTPOT_CONFIGURATION_H
#define WATERUP_PLANTPOT_CONFIGURATION_H

#include <Arduino.h>
#include <EEPROMex.h>
#include <EEPROMPlusPlus.h>

struct Settings
{
    // LedSettings:
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    // MQTTSettings:
    long statisticPublishInterval;
    long resendWarningInterval;
    long pingBrokerInterval;
    uint8_t publishReservoirWarningThreshold;
    // PlantCareSettings:
    long statisticPublishInterval;
    long resendWarningInterval;
    long pingBrokerInterval;
};

struct LedSettings
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct MQTTSettings
{
    long statisticPublishInterval;
    long resendWarningInterval;
    long pingBrokerInterval;
    uint8_t publishReservoirWarningThreshold;
};

struct PlantCareSettings
{
    long takeMeasurementInterval;
    long sleepAfterGivingWater;
    uint8_t groundMoistureOptimal;
};


class Configuration
{
public:
    void setup();
    void setLedSettings( LedSettings );
    void setMQTTSettings( MQTTSettings mqttSettings );
    void setPlantCareSettings( PlantCareSettings plantCareSettings );
    LedSettings getLedSettings();
    MQTTSettings getMqttSettings();
    PlantCareSettings getPlantCareSettings();

private:
};


#endif //WATERUP_PLANTPOT_CONFIGURATION_H
