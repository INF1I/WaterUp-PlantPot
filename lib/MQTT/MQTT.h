/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_MQTT_MQTT_H
#define WATERUP_MQTT_MQTT_H

#ifndef MQTT_DEBUG_MODE
    #define MQTT_DEBUG_MODE 0
#endif

#define wifiSSID "ASUS-ALWIN" // The SSID of the Wifi network to connect to.
#define wifiPassword "test12345" // The password of the Wifi network.
#define mqttBrokerHost "mqtt.inf1i.ga" // The address of the MQTT broker.
#define mqttBrokerPort 8883 // The port to connect to at the MQTT broker.
#define mqttBrokerUsername "inf1i-plantpot" // The pot's username authenticate at the MQTT broker.
#define mqttBrokerPassword "password" // The pot's password to authenticate at the MQTT broker.
#define publishStatisticsTopic "/publish/statistics" // This MQTT topic is used to publish pot state statistics.
#define publishWarningTopic "/publish/warning" // This MQTT topic is used to publish pot warnings such as empty water reservoir warnings.
#define subscribeConfigTopic "/subscribe/config" // This MQTT topic is used receive new pot configuration.
#define jsonBufferSize 200 // This holds the default string buffer size of json messages.

#include <Arduino.h> // Include this library so we can use the arduino system functions and variables.
#include <ESP8266WiFi.h> // Include this library so we can use the ESP8266 chip's functions.
#include <Adafruit_MQTT.h> // Include this library for securely connecting to the internet using WiFi.
#include <Adafruit_MQTT_Client.h> // Include this library for MQTT communication.
#include "FS.h" // Include this library for access to the ESP8266's file system.
#include <Streaming.h> // Include this library for using the << Streaming operator.
#include "Sensors.h" // Include this library for taking pot mesurments.

class MQTT;

/**
 * This collection is used to set the warning type of an MQTT warning message.
 */
enum WarningType
{
    LOW_RESORVOIR = 1,
    EMPTY_RESORVOIR = 2,
    LOW_MOISTURE_LEVEL = 3,
    HIGH_MOISTURE_LEVEL = 4,
    UNKNOWN_ERROR = 5
};

/**
 * This class is used to publish and receive MQTT messages from and to the broker.
 */
class MQTT
{
private:
    Sensors *sensorsLibrary; // Sensors library instance for taking pot mesurements.
    unsigned long previousMillisStatistics; // The last milliseconds an pot statistic was published to the broker.
    unsigned long statisticMillisInterval = 10000; // The interval for publishing pot statistics to the broker.

    /**
     * This function will attempt to verify the TLS/SSL certificate send from the MQTT broker by its SHA1 fingerprint.
     * If the fingerprint doesn't match the one saved in the mqttInf1iGaFingerprint variable it will halt the execution
     * and print an error message to the serial port.
     */
    void verifyFingerprint();

public:
    /**
     * This function will connect to the MQTT broker if we aren't connected already. It will automatically try
     * to reconnect when the connection is lost.
     */
    void mqttConnect();

    /**
     * This function is used to initiate the Arduino/Huzzah board. It gets executed whenever the board is
     * first powered up or after an rest.
     */
    void setup(Sensors *sensorsLib);

    /**
     * This function is used to publish statistics about the pot's state to the broker.
     */
    void publishPotStatistic();

    /**
     * Function for constructing an JSON pot statistic message.
     * @param groundMoistureLevel The measurement data from the soil humidity sensor.
     * @param waterReservoirLevel The measurement data from the ultra sonar sensor in the water reservoir.
     */
    void buildPotStatisticMessage(long groundMoistureLevel, int waterReservoirLevel);

    /**
     * Function for constructing an JSON pot warning message.
     * @param warningType The type of warning that needs to be send.
     */
    void buildPotStatisticMessage(WarningType warningType);

    /**
     * This function will publish pot statistic messages and warnings.
     */
    void runLoop();
};

#endif // WATERUP_MQTT_MQTT_H