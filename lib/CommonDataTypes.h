//
// Created by FuckMicrosoft on 2-7-2017.
//

#ifndef WATERUP_PLANTPOT_COMMONDATATYPES_H
#define WATERUP_PLANTPOT_COMMONDATATYPES_H

/**
 * Data structure that contains LED configuration.
 */
struct LedSettings
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

/**
 * Data structure that contains MQTT configuration.
 */
struct MQTTSettings
{
    uint32_t statisticPublishInterval;
    uint32_t resendWarningInterval;
    uint32_t pingBrokerInterval;
    uint8_t publishReservoirWarningThreshold;
};

/**
 * Data structure that contains plant care configuration.
 */
struct PlantCareSettings
{
    uint32_t takeMeasurementInterval;
    uint32_t sleepAfterGivingWater;
    uint8_t groundMoistureOptimal;
    uint8_t containsPlant;
};


#endif //WATERUP_PLANTPOT_COMMONDATATYPES_H
