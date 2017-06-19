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

Configuration::Configuration(uint8_t eepromAddressOffset)
{
    this->ledSettingsAddress = eepromAddressOffset;
    this->mqttSettingsAddress = this->ledSettings+sizeof(ledSettings);
    this->plantCareSettingsAddress = this->mqttSettings+sizeof(mqttSettings);
}

void Configuration::setup()
{
    EEPROM.begin(EEPROM_MEMORY_SIZE);
    delay(10);

    readSettings(this->ledSettingsAddress, &this->ledSettings);
    readSettings(this->mqttSettingsAddress, &this->mqttSettings);
    readSettings(this->plantCareSettingsAddress, &this->plantCareSettings);
}

void Configuration::resetToDefaults()
{
    this->setLedSettings(
            DEFAULT_SETTING_LED_RED,
            DEFAULT_SETTING_LED_GREEN,
            DEFAULT_SETTING_LED_BLUE
    );

    this->setMQTTSettings(
            DEFAULT_SETTING_MQTT_PING_INTERVAL,
            DEFAULT_SETTING_MQTT_WARNING_INTERVAL,
            DEFAULT_SETTING_MQTT_PING_INTERVAL,
            DEFAULT_SETTING_MQTT_RESERVOIR_WARNING_THRESHOLD
    );

    this->setPlantCareSettings(
        DEFAULT_SETTING_PLANT_CARE_MEASURE_INTERVAL,
        DEFAULT_SETTING_PLANT_CARE_SLEEP_AFTER_WATER,
        DEFAULT_SETTING_PLANT_CARE_MOISTURE_OPTIMAL
    );
}

void Configuration::setLedSettings(LedSettings settings)
{
    this->ledSettings = settings;
    writeSettings(this->ledSettingsAddress, this->ledSettings);
}

void Configuration::setLedSettings(uint8_t red, uint8_t green, uint8_t blue)
{
    this->setLedSettings((LedSettings) {red, green, blue});
}

void Configuration::setMQTTSettings(MQTTSettings mqttSettings)
{
    this->mqttSettings = mqttSettings;
    writeSettings(this->mqttSettingsAddress, this->mqttSettings);
}

void Configuration::setMQTTSettings(uint16_t statisticPublishInterval, uint16_t resendWarningInterval, uint16_t pingBrokerInterval, uint8_t publishReservoirWarningThreshold)
{
    setLedSettings((MQTTSettings) {statisticPublishInterval, resendWarningInterval, pingBrokerInterval, publishReservoirWarningThreshold});
}

void Configuration::setPlantCareSettings(PlantCareSettings plantCareSettings)
{
    this->plantCareSettings = plantCareSettings;
    writeSettings(this->plantCareSettingsAddress, this->plantCareSettings);
}

void Configuration::setPlantCareSettings(uint16_t takeMeasurementInterval, uint16_t sleepAfterGivingWater, uint8_t groundMoistureOptimal)
{
    this->setPlantCareSettings((PlantCareSettings) {takeMeasurementInterval, sleepAfterGivingWater, groundMoistureOptimal});
}

LedSettings Configuration::getLedSettings()
{

}

MQTTSettings Configuration::getMqttSettings();
PlantCareSettings Configuration::getPlantCareSettings();

LedSettings Configuration::ledSettings;
MQTTSettings Configuration::mqttSettings;
PlantCareSettings Configuration::plantCareSettings;