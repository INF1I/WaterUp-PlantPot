/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 19-06-2017 17:51
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_PLANTPOT_CONFIGURATION_H
#define WATERUP_PLANTPOT_CONFIGURATION_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Streaming.h>

#define DEBUG_CONFIG 1

#define EEPROM_MEMORY_SIZE 512 // The size in bytes of the EEPROM memory (512 for the huzzah).
#define DEFAULT_EEPROM_ADDRESS_OFFSET 0 // The addess offset of the config storage.

#define DEFAULT_SETTING_LED_RED 255 // The default setting for the red led.
#define DEFAULT_SETTING_LED_GREEN 255 // The default setting for the green led.
#define DEFAULT_SETTING_LED_BLUE 255 // The default setting for the blue led.

#define DEFAULT_SETTING_MQTT_STATISTIC_INTERVAL 60 // The default statistic publishing interval setting.
#define DEFAULT_SETTING_MQTT_WARNING_INTERVAL 7400 // The default warning resend interval setting.
#define DEFAULT_SETTING_MQTT_PING_INTERVAL 60 // The default ping to MQTT broker interval setting.
#define DEFAULT_SETTING_MQTT_RESERVOIR_WARNING_THRESHOLD 30 // The default threshold for publishing low water reservoir messages setting

#define DEFAULT_SETTING_PLANT_CARE_MEASURE_INTERVAL 60 // The default pot measurement interval setting.
#define DEFAULT_SETTING_PLANT_CARE_SLEEP_AFTER_WATER 1800 // The default sleep time after giving water setting.
#define DEFAULT_SETTING_PLANT_CARE_MOISTURE_OPTIMAL 30 // The default optimal ground moisture level setting.

/**
 * This template simplifies the writing to EEPROM storage of complex data structures.
 *
 * @param startAddress The EEPROM starting address of the data structure.
 * @param value The data structure to write.
 * @return The last address written to.
 */
/*
template<class T> int writeSettings(int startAddress, const T& value)
{
    const byte* p = (const byte*) (const void*) &value;
    unsigned int currentAddress;

    for (currentAddress = 0; currentAddress<sizeof(value); currentAddress++)
    {
        EEPROM.write(startAddress++, *p++);
    }
    return currentAddress;
}
*/

/**
 * This template simplifies the reading from EEPROM storage of complex data structures.
 *
 * @param startAddress The EEPROM starting address of the data structure.
 * @param value The data structure stored in EEPROM.
 * @return The last address read from.
 */
/*template<class T> int readSettings(int startAddress, T& value)
{
    byte* p = (byte*) (void*) &value;
    unsigned int currentAddress;

    for (currentAddress = 0; currentAddress<sizeof(value); currentAddress++)
    {
        *p++ = EEPROM.read(startAddress++);
    }
    return currentAddress;
}*/

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
    uint16_t statisticPublishInterval;
    uint16_t resendWarningInterval;
    uint16_t pingBrokerInterval;
    uint8_t publishReservoirWarningThreshold;
};

/**
 * Data structure that contains plant care configuration.
 */
struct PlantCareSettings
{
    uint16_t takeMeasurementInterval;
    uint16_t sleepAfterGivingWater;
    uint8_t groundMoistureOptimal;
};

/**
 * This class is used to store pot configuration to the EEPROM so it persists
 * when the power is turned off.
 */
class Configuration
{
public:
    Configuration();
    Configuration(uint8_t eepromAddressOffset);

    void setup();
    void resetToDefaults();

    uint8_t getStartAddress();
    uint8_t getEndAddress();

    void setLedSettings( LedSettings settings);
    void setLedSettings(uint8_t red, uint8_t green, uint8_t blue);

    void setMQTTSettings(MQTTSettings settings);
    void setMQTTSettings(uint16_t statisticPublishInterval, uint16_t resendWarningInterval, uint16_t pingBrokerInterval, uint8_t publishReservoirWarningThreshold);

    void setPlantCareSettings(PlantCareSettings settings);
    void setPlantCareSettings(uint16_t takeMeasurementInterval, uint16_t sleepAfterGivingWater, uint8_t groundMoistureOptimal);

    LedSettings getLedSettings();
    MQTTSettings getMqttSettings();
    PlantCareSettings getPlantCareSettings();

    void printConfiguration();
    void printLedConfiguration();
    void printMqttConfiguration();
    void printPlantCareConfiguration();
    void printStorageAddresses();
    void memoryDump( int start = 0, int end = EEPROM_MEMORY_SIZE );
    void clearEEPROM();

private:
    LedSettings *ledSettings;
    MQTTSettings *mqttSettings;
    PlantCareSettings *plantCareSettings;

    uint8_t ledSettingsAddress;
    uint8_t mqttSettingsAddress;
    uint8_t plantCareSettingsAddress;

    uint8_t configurationStartAddress;
    uint8_t configurationEndAddress;

};

#endif //WATERUP_PLANTPOT_CONFIGURATION_H
