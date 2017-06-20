/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 19-06-2017 17:51
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Configuration.h"

LedSettings ledSettingsObject;
MQTTSettings mqttSettingsObject;
PlantCareSettings plantCareSettingsObject;

Configuration::Configuration()
{
    this->ledSettingsAddress = DEFAULT_EEPROM_ADDRESS_OFFSET;
    this->mqttSettingsAddress = this->ledSettingsAddress+sizeof(LedSettings);
    this->plantCareSettingsAddress = this->mqttSettingsAddress+sizeof(MQTTSettings);
    this->configurationStartAddress = this->ledSettingsAddress;
    this->configurationEndAddress = this->plantCareSettingsAddress+sizeof(PlantCareSettings);
}

Configuration::Configuration(uint8_t eepromAddressOffset)
{
    this->ledSettingsAddress = eepromAddressOffset;
    this->mqttSettingsAddress = this->ledSettingsAddress+sizeof(LedSettings);
    this->plantCareSettingsAddress = this->mqttSettingsAddress+sizeof(MQTTSettings);
    this->configurationStartAddress = this->ledSettingsAddress;
    this->configurationEndAddress = this->plantCareSettingsAddress+sizeof(PlantCareSettings);
}

void Configuration::setup()
{
    EEPROM.begin(EEPROM_MEMORY_SIZE);
    delay(10);
}

void Configuration::resetToDefaults()
{
    Serial << F("[debug] - Reseting the EEPROM configuration to the defaults") << endl;
    ledSettingsObject.red = (uint8_t)DEFAULT_SETTING_LED_RED;
    ledSettingsObject.green = (uint8_t)DEFAULT_SETTING_LED_GREEN;
    ledSettingsObject.blue = (uint8_t)DEFAULT_SETTING_LED_BLUE;

    mqttSettingsObject.statisticPublishInterval = (uint16_t)DEFAULT_SETTING_MQTT_STATISTIC_INTERVAL;
    mqttSettingsObject.resendWarningInterval = (uint16_t)DEFAULT_SETTING_MQTT_WARNING_INTERVAL;
    mqttSettingsObject.pingBrokerInterval = (uint16_t)DEFAULT_SETTING_MQTT_PING_INTERVAL;
    mqttSettingsObject.publishReservoirWarningThreshold = (uint8_t)DEFAULT_SETTING_MQTT_RESERVOIR_WARNING_THRESHOLD;

    plantCareSettingsObject.takeMeasurementInterval = (uint16_t)DEFAULT_SETTING_PLANT_CARE_MEASURE_INTERVAL;
    plantCareSettingsObject.sleepAfterGivingWater = (uint16_t)DEFAULT_SETTING_PLANT_CARE_SLEEP_AFTER_WATER;
    plantCareSettingsObject.groundMoistureOptimal = (uint8_t)DEFAULT_SETTING_PLANT_CARE_MOISTURE_OPTIMAL;

#ifdef DEBUG_CONFIG
    this->printConfiguration();
#endif
}

uint8_t Configuration::getStartAddress()
{
    return this->configurationStartAddress;
}

uint8_t Configuration::getEndAddress()
{
    return this->configurationEndAddress;
}

void Configuration::setLedSettings(LedSettings settings)
{
    this->setLedSettings( settings.red, settings.green, settings.blue );
}

void Configuration::setLedSettings(uint8_t red, uint8_t green, uint8_t blue)
{
    ledSettingsObject.red = red;
    ledSettingsObject.green = green;
    ledSettingsObject.blue = blue;
}

void Configuration::setMQTTSettings(MQTTSettings settings)
{
    this->setMQTTSettings(settings.statisticPublishInterval, settings.resendWarningInterval, settings.pingBrokerInterval, settings.publishReservoirWarningThreshold);
}

void Configuration::setMQTTSettings(uint16_t statisticPublishInterval, uint16_t resendWarningInterval, uint16_t pingBrokerInterval, uint8_t publishReservoirWarningThreshold)
{
    mqttSettingsObject.statisticPublishInterval = statisticPublishInterval;
    mqttSettingsObject.resendWarningInterval = resendWarningInterval;
    mqttSettingsObject.pingBrokerInterval = pingBrokerInterval;
    mqttSettingsObject.publishReservoirWarningThreshold = publishReservoirWarningThreshold;
}

void Configuration::setPlantCareSettings(PlantCareSettings settings)
{
    this->setPlantCareSettings( settings.takeMeasurementInterval, settings.sleepAfterGivingWater, settings.groundMoistureOptimal );
}

void Configuration::setPlantCareSettings(uint16_t takeMeasurementInterval, uint16_t sleepAfterGivingWater, uint8_t groundMoistureOptimal)
{
    plantCareSettingsObject.takeMeasurementInterval = takeMeasurementInterval;
    plantCareSettingsObject.sleepAfterGivingWater = sleepAfterGivingWater;
    plantCareSettingsObject.groundMoistureOptimal = groundMoistureOptimal;
}

LedSettings Configuration::getLedSettings()
{
    return ledSettingsObject;
}

MQTTSettings Configuration::getMqttSettings()
{
    return mqttSettingsObject;
}

PlantCareSettings Configuration::getPlantCareSettings()
{
    return plantCareSettingsObject;
}

void Configuration::printConfiguration()
{
    Serial << F("[debug] - Printing all configuration:") << endl;
    Serial << F("LED settings = {\n\tred:") << ledSettingsObject.red << F(",\n\tgreen:") << ledSettingsObject.green << F(",\n\tblue:") << ledSettingsObject.blue << F("\n};\n");
    Serial << F("MQTT settings = {\n\tpingBrokerInterval:") << mqttSettingsObject.pingBrokerInterval << F(",\n\tresendWarningInterval:") << mqttSettingsObject.resendWarningInterval << F(",\n\tstatisticPublishInterval:") << mqttSettingsObject.statisticPublishInterval << F(",\n\tresendWarningInterval:") << mqttSettingsObject.resendWarningInterval << F("\n};\n");
    Serial << F("Plant Care settings = {\n\ttakeMeasurementInterval:") << plantCareSettingsObject.takeMeasurementInterval << F(",\n\tsleepAfterGivingWater:") << plantCareSettingsObject.sleepAfterGivingWater << F(",\n\tgroundMoistureOptimal:") << plantCareSettingsObject.groundMoistureOptimal << F("\n};\n");
}

void Configuration::printLedConfiguration()
{
    Serial << F("[debug] - Printing LED configuration:") << endl;
    Serial << F("LED settings = {\n\tred:") << ledSettingsObject.red << F(",\n\tgreen:") << ledSettingsObject.green << F(",\n\tblue:") << ledSettingsObject.blue << F("\n};\n");
}

void Configuration::printMqttConfiguration()
{
    Serial << F("[debug] - Printing MQTT configuration:") << endl;
    Serial << F("MQTT settings = {\n\tstatisticPublishInterval:") << mqttSettingsObject.statisticPublishInterval << F(",\n\tresendWarningInterval:") << mqttSettingsObject.resendWarningInterval << F(",\n\tpingBrokerInterval:") << mqttSettingsObject.pingBrokerInterval << F(",\n\tpublishReservoirWarningThreshold:") << mqttSettingsObject.publishReservoirWarningThreshold << F("\n};\n");
}

void Configuration::printPlantCareConfiguration()
{
    Serial << F("[debug] - Printing plant care configuration:") << endl;
    Serial << F("Plant Care settings = {\n\ttakeMeasurementInterval:") << plantCareSettingsObject.takeMeasurementInterval << F(",\n\tsleepAfterGivingWater:") << plantCareSettingsObject.sleepAfterGivingWater << F(",\n\tgroundMoistureOptimal:") << plantCareSettingsObject.groundMoistureOptimal << F("\n};\n");
}

void Configuration::printStorageAddresses()
{
    Serial << F("[debug] - Enabled memory address debugging mode for Configuration library.") << endl;
    Serial << F("[debug] - Configuration library EEPROM start address: ") << this->configurationStartAddress << endl;
    Serial << F("[debug] - Configuration library EEPROM Led config start address: ") << this->ledSettingsAddress << endl;
    Serial << F("[debug] - Configuration library EEPROM MQTT config start address: ") << this->mqttSettingsAddress << endl;
    Serial << F("[debug] - Configuration library EEPROM plant care config start address: ") << this->plantCareSettingsAddress << endl;
    Serial << F("[debug] - Configuration library EEPROM end address: ") << this->configurationEndAddress << endl;
}

void Configuration::memoryDump( int start, int end )
{
/*    Serial << "[debug] - Creating an EEPROM raw memory dump:" << endl;

    for( int i = start; i <= end; i++ )
    {
        Serial << "EEPROM[" << i << "] = " << EEPROM.get(i) << ";" << endl;
    }
    Serial << "[debug] - End of memory dump" << endl;*/
}

void Configuration::clearEEPROM()
{
   /* Serial << "[debug] - Enabled memory debugging mode for Configuration library." << endl;
    for (int i = 0 ; i <= (EEPROM_MEMORY_SIZE/8) ; i++)
    {
        EEPROM.put(i, 0);
    }*/
}