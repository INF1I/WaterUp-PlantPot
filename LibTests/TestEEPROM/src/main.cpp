/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 19-06-2017 18:48
 * Licence: GPLv3 - General Public Licence version 3
 */
#include <Arduino.h>
#include <Streaming.h>
#include <Configuration.h>

Configuration configuration;

void printDevider( String title )
{
    for(int i=0; i<50; i++)
    {
        Serial << F("*");
    }
    Serial << F("\n*\t\t") << title << endl;
    for(int i=0; i<50; i++)
    {
        Serial << F("*");
    }
    Serial << endl;
}

void setup()
{
    Serial.begin(9600);
    delay(5000);
    Serial << "Setting up configuration library" << endl;
    configuration.setup();
}

void loop()
{
    delay(10000);
    configuration.printConfiguration();
    Serial << F("Set all objects to defaults:") << endl;
    configuration.resetToDefaults();

    printDevider("Test update LED settings");
    configuration.setLedSettings( (LedSettings){ 50, 50, 50 } );
    configuration.setLedSettings( 123, 132, 231 );

    printDevider("Test update MQTT settings");
    configuration.setMQTTSettings( (MQTTSettings){ 1, 2, 3, 4 } );
    configuration.setMQTTSettings( 4, 3, 2, 1 );

    printDevider("Test update plant care settings");
    configuration.setPlantCareSettings( (PlantCareSettings){ 40, 30, 20 } );
    configuration.setPlantCareSettings( 20, 30, 40 );

    Serial << F("[debug] - End of loop iteration.") << endl;
}

/*
int configAddress = 0;
int setupAddress;

Configuration1 config;
Configuration configLib;
Settings setting;

void doSomeThing( Settings settings );
void printSize();
void printEEPROM();
void loadAndPrintDefaults();
void printDefaultConsts();
void printAddrInfo();

void setup()
{
    Serial.begin(9600);
    configLib.setup();
}

void loop()
{
    delay(10000);
    configLib.memoryDump(0,50);
    configLib.clearEEPROM();
    configLib.debugEepromAddresses();
    configLib.memoryDump(0,50);

    printDefaultConsts();
    delay(2000);
    loadAndPrintDefaults();
    configLib.memoryDump(0,50);
    delay(2000);
    printAddrInfo();
}

void printDefaultConsts()
{
    Serial << "Default configuration:" << endl;
    Serial << "defaults = {\n\tred:" << (uint8_t)DEFAULT_SETTING_LED_RED << ",\n\tgreen:" << (uint8_t)DEFAULT_SETTING_LED_GREEN << ",\n\tblue:" << (uint8_t)DEFAULT_SETTING_LED_BLUE << "\n};\n";
}

void loadAndPrintDefaults()
{
    configLib.resetToDefaults();
    Serial << "Loaded default configuration:" << endl;
    LedSettings *ledSettings = configLib.getLedSettings();
    Serial << "Printing settings: LED{\n\tred:" << ledSettings->red << ",\n\tgreen:" << ledSettings->green << ",\n\tblue:" << ledSettings->blue << "\n};\n" << endl;
}

void printAddrInfo()
{
    Serial << "Printing mem address info:" << endl;
    Serial << "Addresses = {\n\tconf_start:" << configLib.getStartAddress() << ",\n\tconf_end:" << configLib.getEndAddress() << "\n};" << endl;
}

void doSomeThing( Settings settings )
{
    Serial << "Start:" << settings.start << " Stop:" << settings.stop << " Hello:" << settings.hello << endl;
}

void printEEPROM()
{
    Serial << F("Values stored in EEPROM:") << endl;
    Serial << F("Config Address:") << configAddress << F("| Setup Address:") << setupAddress << endl;
    EEPROM_readAnything( configAddress, config);
    EEPROM_readAnything( sizeof(config), setting );
    Serial << "Red:" << config.red << " Green:" << config.green << " Blue:" << config.blue << endl;
    Serial << "Start:" << setting.start << " Stop:" << setting.stop << " Hello:" << setting.hello << endl;
}

void printSize()
{
    Serial << "int:" << sizeof(int) << endl;
    Serial << "uint8_t:" << sizeof(uint8_t) << endl;
    Serial << "uint16_t:" << sizeof(uint16_t) << endl;
    Serial << "long:" << sizeof(long) << endl << endl;
    Serial << "byte:" << sizeof(byte) << endl;
    Serial << "char:" << sizeof(char) << endl;
    Serial << "unsigned long:" << sizeof(unsigned long) << endl;
    Serial << "unsigned int:" << sizeof(unsigned int) << endl;
}*/
 /*
#include <Arduino.h>
#include <Configuration.h>
#include <Streaming.h>

Configuration config;

void setup()
{
    delay(10000);
    Serial.begin(9600);
    Serial << "Setting up stuff" << endl;
    config.setup();
    Serial << "Setting up stuff" << endl;
    config.resetToDefaults();
}

void loop()
{
    delay(10000);
    Serial << "Loaded default configuration:" << endl;
    LedSettings *ledSettings = config.getLedSettings();
    Serial << "Printing settings: LED{\n";
    Serial << "red:" << ledSettings->red << ",\ngreen:" << ledSettings->green << ",\nblue:" << ledSettings->blue << "\n}" << endl;
    Serial << "Printing settings: MQTT" << endl;
    Serial << "Printing settings: PLANT CARE" << endl;
}
*/