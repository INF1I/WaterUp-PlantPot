/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 19-06-2017 17:51
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Configuration.h"

LedSettings ledSettingsObject{};
MQTTSettings mqttSettingsObject{};
PlantCareSettings plantCareSettingsObject{};

Configuration::Configuration()
{
#ifdef DEBUG_CONFIG
    delay(5000); // Wait 5 seconds so you have time to start the serial monitor.
    Serial << "[debug] - Enabled debugging mode for Configuration library." << endl;
    Serial << "[debug] - Configuration library EEPROM start address: " << DEFAULT_EEPROM_ADDRESS_OFFSET << endl;
#endif
    this->ledSettingsAddress = DEFAULT_EEPROM_ADDRESS_OFFSET;
#ifdef DEBUG_CONFIG
    Serial << "[debug] - Configuration library EEPROM Led config start address: " << this->ledSettingsAddress << endl;
#endif
    this->mqttSettingsAddress = this->ledSettingsAddress+sizeof(LedSettings);
#ifdef DEBUG_CONFIG
    Serial << "[debug] - Configuration library EEPROM MQTT config start address: " << this->mqttSettingsAddress << endl;
#endif
    this->plantCareSettingsAddress = this->mqttSettingsAddress+sizeof(MQTTSettings);
#ifdef DEBUG_CONFIG
    Serial << "[debug] - Configuration library EEPROM plant care config start address: " << this->plantCareSettingsAddress << endl;
#endif
    this->configurationStartAddress = this->ledSettingsAddress;
    this->configurationEndAddress = this->plantCareSettingsAddress+sizeof(PlantCareSettings);
#ifdef DEBUG_CONFIG
    Serial << "[debug] - Configuration library EEPROM end address: " << this->configurationEndAddress << endl;
#endif
}

Configuration::Configuration(uint8_t eepromAddressOffset)
{
#ifdef DEBUG_CONFIG
    delay(5000); // Wait 5 seconds so you have time to start the serial monitor.
    Serial << "[debug] - Enabled debugging mode for Configuration library." << endl;
    Serial << "[debug] - Configuration library EEPROM start address: " << eepromAddressOffset << endl;
#endif
    this->ledSettingsAddress = eepromAddressOffset;
#ifdef DEBUG_CONFIG
    Serial << "[debug] - Configuration library EEPROM Led config start address: " << this->ledSettingsAddress << endl;
#endif
    this->mqttSettingsAddress = this->ledSettingsAddress+sizeof(LedSettings);
#ifdef DEBUG_CONFIG
    Serial << "[debug] - Configuration library EEPROM MQTT config start address: " << this->mqttSettingsAddress << endl;
#endif
    this->plantCareSettingsAddress = this->mqttSettingsAddress+sizeof(MQTTSettings);
#ifdef DEBUG_CONFIG
    Serial << "[debug] - Configuration library EEPROM plant care config start address: " << this->plantCareSettingsAddress << endl;
#endif
    this->configurationStartAddress = this->ledSettingsAddress;
    this->configurationEndAddress = this->plantCareSettingsAddress+sizeof(PlantCareSettings);
#ifdef DEBUG_CONFIG
    Serial << "[debug] - Configuration library EEPROM end address: " << this->configurationEndAddress << endl;
#endif
}

void Configuration::setup()
{
    EEPROM.begin(EEPROM_MEMORY_SIZE);
    delay(10);

    setLedSettings( &ledSettingsObject );
    setMQTTSettings( &mqttSettingsObject );
    setPlantCareSettings( &plantCareSettingsObject );

    readSettings(this->ledSettingsAddress, this->ledSettings);
    readSettings(this->mqttSettingsAddress, this->mqttSettings);
    readSettings(this->plantCareSettingsAddress, this->plantCareSettings);
}

void Configuration::resetToDefaults()
{
    this->setLedSettings(
            (uint8_t)DEFAULT_SETTING_LED_RED,
            (uint8_t)DEFAULT_SETTING_LED_GREEN,
            (uint8_t)DEFAULT_SETTING_LED_BLUE
    );

    this->setMQTTSettings(
            (uint16_t)DEFAULT_SETTING_MQTT_PING_INTERVAL,
            (uint16_t)DEFAULT_SETTING_MQTT_WARNING_INTERVAL,
            (uint16_t)DEFAULT_SETTING_MQTT_PING_INTERVAL,
            (uint8_t)DEFAULT_SETTING_MQTT_RESERVOIR_WARNING_THRESHOLD
    );

    this->setPlantCareSettings(
            (uint16_t)DEFAULT_SETTING_PLANT_CARE_MEASURE_INTERVAL,
            (uint16_t)DEFAULT_SETTING_PLANT_CARE_SLEEP_AFTER_WATER,
            (uint8_t)DEFAULT_SETTING_PLANT_CARE_MOISTURE_OPTIMAL
    );
}

uint8_t Configuration::getStartAddress()
{
    return this->configurationStartAddress;
}

uint8_t Configuration::getEndAddress()
{
    return this->configurationEndAddress;
}

void Configuration::setLedSettings(LedSettings* settings)
{
    this->ledSettings = settings;
    writeSettings(this->ledSettingsAddress, this->ledSettings);
}

void Configuration::setLedSettings(uint8_t red, uint8_t green, uint8_t blue)
{
    this->ledSettings->red = red;
    this->ledSettings->green = green;
    this->ledSettings->blue = blue;
}

void Configuration::setMQTTSettings(MQTTSettings* mqttSettings)
{
    this->mqttSettings = mqttSettings;
    writeSettings(this->mqttSettingsAddress, this->mqttSettings);
}

void Configuration::setMQTTSettings(uint16_t statisticPublishInterval, uint16_t resendWarningInterval, uint16_t pingBrokerInterval, uint8_t publishReservoirWarningThreshold)
{
    this->mqttSettings->statisticPublishInterval = statisticPublishInterval;
    this->mqttSettings->resendWarningInterval = resendWarningInterval;
    this->mqttSettings->pingBrokerInterval = pingBrokerInterval;
    this->mqttSettings->publishReservoirWarningThreshold = publishReservoirWarningThreshold;
}

void Configuration::setPlantCareSettings(PlantCareSettings* plantCareSettings)
{
    this->plantCareSettings = plantCareSettings;
    writeSettings(this->plantCareSettingsAddress, this->plantCareSettings);
}

void Configuration::setPlantCareSettings(uint16_t takeMeasurementInterval, uint16_t sleepAfterGivingWater, uint8_t groundMoistureOptimal)
{
    this->plantCareSettings->takeMeasurementInterval = takeMeasurementInterval;
    this->plantCareSettings->sleepAfterGivingWater = sleepAfterGivingWater;
    this->plantCareSettings->groundMoistureOptimal = groundMoistureOptimal;
}

LedSettings *Configuration::getLedSettings()
{
    //readSettings(this->ledSettingsAddress, this->ledSettings);
    return this->ledSettings;
}

MQTTSettings *Configuration::getMqttSettings()
{
    //readSettings(this->mqttSettingsAddress, this->mqttSettings);
    return this->mqttSettings;
}

PlantCareSettings *Configuration::getPlantCareSettings()
{
    //readSettings(this->plantCareSettingsAddress, this->plantCareSettings);
    return this->plantCareSettings;
}

void Configuration::debugEepromAddresses()
{
    Serial << "[debug] - Enabled memory address debugging mode for Configuration library." << endl;
    Serial << "[debug] - Configuration library EEPROM start address: " << this->configurationStartAddress << endl;
    Serial << "[debug] - Configuration library EEPROM Led config start address: " << this->ledSettingsAddress << endl;
    Serial << "[debug] - Configuration library EEPROM MQTT config start address: " << this->mqttSettingsAddress << endl;
    Serial << "[debug] - Configuration library EEPROM plant care config start address: " << this->plantCareSettingsAddress << endl;
    Serial << "[debug] - Configuration library EEPROM end address: " << this->configurationEndAddress << endl;
}

void Configuration::memoryDump( int start, int end )
{
    Serial << "[debug] - Creating an EEPROM raw memory dump:" << endl;

    for( int i = start; i <= end; i++ )
    {
        Serial << "EEPROM[" << i << "] = " << EEPROM.read(i) << ";" << endl;
    }
    Serial << "[debug] - End of memory dump" << endl;
}

void Configuration::clearEEPROM()
{
    Serial << "[debug] - Enabled memory debugging mode for Configuration library." << endl;
    for (int i = 0 ; i <= EEPROM_MEMORY_SIZE ; i++)
    {
        EEPROM.write(i, 0);
    }
}