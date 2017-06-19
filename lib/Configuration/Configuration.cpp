/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 19-06-2017 17:51
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Configuration.h"

Configuration::Configuration()
{
    this->ledSettingsAddress = DEFAULT_EEPROM_ADDRESS_OFFSET;
    this->mqttSettingsAddress = this->ledSettings+sizeof(ledSettings);
    this->plantCareSettingsAddress = this->mqttSettings+sizeof(mqttSettings);
}

Configuration::Configuration( uint8_t eepromAddressOffset )
{
    this->ledSettingsAddress = eepromAddressOffset;
    this->mqttSettingsAddress = this->ledSettings+sizeof(ledSettings);
    this->plantCareSettingsAddress = this->mqttSettings+sizeof(mqttSettings);
}

void Configuration::setup()
{
    EEPROM.begin( EEPROM_MEMORY_SIZE );
    delay( 10 );

    readSettings( this->ledSettingsAddress, &this->ledSettings );
    readSettings( this->mqttSettingsAddress, &this->mqttSettings );
    readSettings( this->plantCareSettingsAddress, &this->plantCareSettings );
}

void Configuration::resetToDefaults()
{

}

void Configuration::setLedSettings(LedSettings)
{

}

void Configuration::setLedSettings(uint8_t red, uint8_t green, uint8_t blue)
{

}

void Configuration::setMQTTSettings(MQTTSettings mqttSettings)
{

}

void Configuration::setMQTTSettings(uint16_t statisticPublishInterval, uint16_t resendWarningInterval, uint16_t pingBrokerInterval, uint8_t publishReservoirWarningThreshold)
{

}

void Configuration::setPlantCareSettings(PlantCareSettings plantCareSettings)
{

}

void Configuration::setPlantCareSettings(uint16_t takeMeasurementInterval, uint16_t sleepAfterGivingWater, uint8_t groundMoistureOptimal)
{

}

LedSettings Configuration::getLedSettings();
MQTTSettings Configuration::getMqttSettings();
PlantCareSettings Configuration::getPlantCareSettings();

LedSettings Configuration::ledSettings;
MQTTSettings Configuration::mqttSettings;
PlantCareSettings Configuration::plantCareSettings;