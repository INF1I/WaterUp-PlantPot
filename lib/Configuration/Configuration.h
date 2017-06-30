/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 19-06-2017 17:51
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_PLANTPOT_CONFIGURATION_H
#define WATERUP_PLANTPOT_CONFIGURATION_H

#include <Arduino.h> // Include this library for using basic system functions and variables.
#include <Streaming.h> // Include this library for using the << Streaming operator.
#include <ESP8266WiFi.h> // Include this library for working with the ESP8266 chip.
#include <EEPROM.h> // Include this library for using the EEPROM flas storage on the huzzah.
#include <WiFiManager.h> // Include this library for dynamically setting up the WiFi connection.
#include <Adafruit_MQTT.h> // Include this library for securely connecting to the internet using WiFi.
#include <Adafruit_MQTT_Client.h> // Include this library for MQTT communication.
#include <ArduinoJson.h> // Include this library for parsing incomming json mesages.
//#include <Configuration.h> // This library contains the code for loading plant pot configuration.
#include <Communication.h> // This library contains the code for communication between the pot and broker.
#include <PlantCare.h> // This library contains the code for taking care of the plant.
#include <LedController.h> // This library contains the code for taking care of the plant.

#define EEPROM_MEMORY_SIZE 512 // The size in bytes of the EEPROM memory (512 for the huzzah).
#define DEFAULT_EEPROM_ADDRESS_OFFSET 0 // The addess offset of the config storage.

#define DEFAULT_SETTING_LED_RED 255 // The default setting for the red led.
#define DEFAULT_SETTING_LED_GREEN 255 // The default setting for the green led.
#define DEFAULT_SETTING_LED_BLUE 255 // The default setting for the blue led.

#define DEFAULT_SETTING_MQTT_STATISTIC_INTERVAL 10000 // The default statistic publishing interval setting.
#define DEFAULT_SETTING_MQTT_WARNING_INTERVAL 7200000 // The default warning resend interval setting.
#define DEFAULT_SETTING_MQTT_PING_INTERVAL 60000 // The default ping to MQTT broker interval setting.
#define DEFAULT_SETTING_MQTT_RESERVOIR_WARNING_THRESHOLD 30 // The default threshold for publishing low water reservoir messages setting

#define DEFAULT_SETTING_PLANT_CARE_MEASURE_INTERVAL 60000 // The default pot measurement interval setting.
#define DEFAULT_SETTING_PLANT_CARE_SLEEP_AFTER_WATER 3600000 // The default sleep time after giving water setting.
#define DEFAULT_SETTING_PLANT_CARE_MOISTURE_OPTIMAL 30 // The default optimal ground moisture level setting.

class Communication; // Forward declare the communication library.
class Configuration; //  Forward declare the configuration library.
class PlantCare; // Forward declare the plant care library.

/**
 * 
 * This template simplifies the writing to EEPROM storage of complex data structures.
 *
 * @param startAddress The EEPROM starting address of the data structure.
 * @param value The data structure to write.
 * @return The last address written to.
 */
template<class T> int writeSettings(int startAddress, const T& value)
{
    Serial << "[debug] - Write operation" << endl;
    const byte* p = (const byte*) (const void*) &value;
    unsigned int currentAddress;

    for (currentAddress = 0; currentAddress<sizeof(value); currentAddress++)
    {
        EEPROM.write(startAddress++, *p++);
    }
    return currentAddress;
}

/**
 * This template simplifies the reading from EEPROM storage of complex data structures.
 *
 * @param startAddress The EEPROM starting address of the data structure.
 * @param value The data structure stored in EEPROM.
 * @return The last address read from.
 */
template<class T> int readSettings(int startAddress, T& value)
{
    Serial << "[debug] - Read operation" << endl;
    byte* p = (byte*) (void*) &value;
    unsigned int currentAddress;

    for (currentAddress = 0; currentAddress<sizeof(value); currentAddress++)
    {
        *p++ = EEPROM.read(startAddress++);
    }
    return currentAddress;
}
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
};


/**
 * This class is used to store pot configuration to the EEPROM so it persists
 * when the power is turned off.
 */
class Configuration
{
public:
    /**
     * An enumeration containing all possible warning types.
     */
    enum WarningType
    {
        NO_ERROR = 0,
        LOW_RESERVOIR = 1,
        EMPTY_RESERVOIR = 2,
        UNKNOWN_ERROR = 3
    };

    /**
     * This will initiate the configuration library it will set the eeprom size
     * and default memory addresses used to store configuration.
     */
    Configuration();

    /**
     * This will initiate the EEPROM library and it will load the stored settings into ram.
     */
    void setup();

    /**
     * This will write the settings stored in ram to eeprom memory for persisting configuration
     * through power cicles.
     */
    void store();

    /**
     * This will read the settings stored in eeprom and load it into ram so we can configure and
     * update the pot during its use.
     */
    void load();

    /**
     *  This will load the default configuration and overwrite it with the configuration
     *  stored in ram and persist the new settings to the eeprom memory.
     */
    void reset();

    /**
     * This will iterate through all eeprom memory addresses and write zeros to every address
     * effectively clearing all stored data on the eeprom.
     */
    void clear();

    /**
     * This function accepts the setting for red, green and blue luminosity strength and
     * overwrite it over the configuration stored in ram.
     *
     * @param red       An byte representing the luminosity strength of rhe red led.
     * @param green     An byte representing the luminosity strength of rhe green led.
     * @param blue      An byte representing the luminosity strength of rhe blue led.
     */
    void setLedSettings(uint8_t red, uint8_t green, uint8_t blue);

    /**
     * This function accepts multiple settings about the mqtt communication interval and overwrite
     * the ones stored in ram with it.
     *
     * @param statisticPublishInterval          The interval of publishing statistic messages.
     * @param resendWarningInterval             The interval of republishing warnings to the user.
     * @param pingBrokerInterval                The interval of pinging to the broker.
     * @param publishReservoirWarningThreshold  The threshold of sending an low water level warning too the user.
     */
    void setMQTTSettings(uint32_t statisticPublishInterval, uint32_t resendWarningInterval, uint32_t pingBrokerInterval, uint8_t publishReservoirWarningThreshold);

    /**
     * This function accepts some basic plant care settings as argument and will overwrite them
     * with the ones stored in ram.
     *
     * @param takeMeasurementInterval   The interval of taking soil moisture and water reservoir level measurements.
     * @param sleepAfterGivingWater     The time to wait with giving water after it gave some water.
     * @param groundMoistureOptimal     The optimal percentage of soil moisture for the current plant.
     */
    void setPlantCareSettings(uint32_t takeMeasurementInterval, uint32_t sleepAfterGivingWater, uint8_t groundMoistureOptimal);

    /**
     * This gets the LedSettings struct address currently in use and stored in ram.
     *
     * @return LedSettings* an pointer to the led settings struct.
     */
    LedSettings* getLedSettings();

    /**
     * This gets the MQTTSettings struct address currently in use and stored in ram.
     *
     * @return MQTTSettings* an pointer to the mqtt settings struct.
     */
    MQTTSettings* getMqttSettings();

    /**
     * This gets the PlantCareSettings struct address currently in use and stored in ram.
     *
     * @return PlantCareSettings* an pointer to the plant care settings struct.
     */
    PlantCareSettings* getPlantCareSettings();

    /**
     * This function will print all the current configuration stored in ram.
     */
    void printConfiguration();

    /**
     * This function will print the current led configuration stored in ram.
     */
    void printLedConfiguration();

    /**
     * This function will print the current mqtt configuration stored in ram.
     */
    void printMqttConfiguration();

    /**
     * This function will print the current plant care configuration stored in ram.
     */
    void printPlantCareConfiguration();

    /**
     * This function will print all the eeprom memory addresses used to permanently
     * store configuration on the pot.f
     */
    void printStorageAddresses();

    /**
     * This function will print an complete memory dump of all data stored on the
     * eeprom memory.
     */
    void printMemoryDump();

    /**
     * This function will print an ranged memory dump of some of the data stored on
     * the eeprom memory.
     *
     * @param start     The starting address of the memory to dump.
     * @param end       The ending address of the memory to dump.
     */
    void printMemoryDump(uint8_t start, uint8_t end);

    /**
     * This function will print an memory dump of all addresses used to store pot
     * configuration.
     */
    void printMemory();

    /**
     * This function will print an memory dump of all addresses used to store led
     * configuration.
     */
    void printLedMemory();

    /**
     * This function will print an memory dump of all addresses used to store mqtt
     * configuration.
     */
    void printMqttMemory();

    /**
     * This function will print an memory dump of all addresses used to store plant
     * care configuration.
     */
    void printPlantCareMemory();

private:
    uint16_t eepromSize; // The amount of bits available on the eeprom storage.
    uint8_t configurationStartAddress; // The eeprom starting address of the configuration.
    uint8_t configurationEndAddress; // The eeprom ending address of the configuration.
    uint8_t ledSettingsAddress; // The eeprom starting address of the led configuration.
    uint8_t mqttSettingsAddress; // The eerpom starting address of the mqtt configuration.
    uint8_t plantCareSettingsAddress; // The eeprom starting address of the plant care configuration.

    /**
     * This functions returns the size of the eeprom storage.
     * @return  The size in bits of the eerpom storage.
     */
    uint16_t getEepromSize();

    /**
     * Thhis function returns the starting address of all the pot configuration.
     * @return  An byte containing the start address of all plant configuration.
     */
    uint8_t getConfigurationStartAddress();

    /**
     * This function returns the ending address of all the pot configuration.
     * @return  An byte containing the end address of all plant configuration.
     */
    uint8_t getConfigurationEndAddress();

    /**
     * This function returns the starting address of the led configuration.
     * @return  An byte containing the end address of the led configuration.
     */
    uint8_t getLedSettingsAddress();

    /**
     * This function returns the starting address of the mqtt configuration.
     * @return  An byte containing the end address of the mqtt configuration.
     */
    uint8_t getMqttSettingsAddress();

    /**
     * This function returns the starting address of the plant care configuration.
     * @return  An byte containing the end address of the plant care configuration.
     */
    uint8_t getPlantCareSettingsAddress();
};

#endif //WATERUP_PLANTPOT_CONFIGURATION_H
