/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 17-06-2017 20:16
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_PLANTPOT_COMMUNICATION_H
#define WATERUP_PLANTPOT_COMMUNICATION_H

#include <Arduino.h> // Include this library for using basic system functions and variables.
#include <ESP8266WiFi.h> // Include this library for working with the ESP8266 chip.
#include <WiFiManager.h> // Include this library for dynamically setting up the WiFi connection.
#include <Adafruit_MQTT.h> // Include this library for securely connecting to the internet using WiFi.
#include <Adafruit_MQTT_Client.h> // Include this library for MQTT communication.
#include <ArduinoJson.h> // Include this library for parsing incomming json mesages.
//#include <FS.h> // Include this library for access to the ESP8266's file system.
#include <Streaming.h> // Include this library for using the << Streaming operator.
//#include <EEPROM.h> // Include this library for using the EEPROM flas storage on the huzzah.
#include <Configuration.h> // This library contains the code for loading plant pot configuration.
//#include <Communication.h> // This library contains the code for communication between the pot and broker.
#include <PlantCare.h> // This library contains the code for taking care of the plant.

#define MQTT_BROKER_HOST "mqtt.inf1i.ga" // The address of the MQTT broker.
#define MQTT_BROKER_PORT 8883 // The port to connect to at the MQTT broker.
#define MQTT_BROKER_USERNAME "inf1i-plantpot" // The pot's username authenticate at the MQTT broker.
#define MQTT_BROKER_PASSWORD "password" // The pot's password to authenticate at the MQTT broker.
/**
 * The SHA1 fingerprint taken from the backend server's SSL certificates.
 */
#define MQTT_BROKER_FINGERPRINT "A6 E4 A9 8C 92 B3 8D 81 73 CE 5B 33 33 F5 A3 7A 1B 87 E2 F3"

#define TOPIC_PUBLISH_STATISTIC "/publish/statistic" // This MQTT topic is used to publish pot state statistics.
#define TOPIC_PUBLISH_WARNING "/publish/warning" // This is the MQTT topic used to publis warnings to the user.

#define TOPIC_SUBSCRIBE_LED_CONFIG "/subscribe/config/led" // This is the MQTT topic used to listen for led configuration.
#define TOPIC_SUBSCRIBE_MQTT_CONFIG "/subscribe/config/mqtt" // This is the MQTT topic used to listen for mqtt configuration.
#define TOPIC_SUBSCRIBE_PLANT_CARE_CONFIG "/subscribe/config/plant-care" // This is the MQTT topic used to listen for plant care configuration.

#define JSON_BUFFER_SIZE 200 // This holds the default string buffer size of json messages.

class Communication; // Forward declare the communication library.
class Configuration; //  Forward declare the configuration library.
class PlantCare; // Forward declare the plant care library.

class Communication
    {
    public:
        /**
         * An pointer to the pot configuration object.
         */
        Configuration *potConfig;

        /**
         * The constructor will initiate the communication library with some default
         * values and will save an reference to the configuration library.
         * @param potConfiguration  An pointer to the configuration library.
         */
        Communication( Configuration * potConfiguration );

    /**
     * This function is used to initiate the Arduino/Huzzah board. It gets
     * executed whenever the board is first powered up or after an rest. It will
     * initiate the communication settings and launce a access point if the
     * there are no valid wifi settings stored.
     */
    void setup();

    /**
     * This function is used to check if there is an connection to the mqtt broker.
     * If not it will attempt to pen one.
     */
    void connect();

    /**
     * This function will return the pointer to the configuration object that
     * contains communication and plant care settings.
     *
     * @return *Configuration
     */
    Configuration* getConfiguration();

    /**
     * This function will publish statistics about the pot's current state to the
     * mqtt broker.
     *
     * @param groundMoistureLevel   The current percentage of moisture in the ground.
     * @param waterReservoirLevel   The current percentage of water left in the reservoir.
     */
    void publishStatistic(int groundMoistureLevel, int waterReservoirLevel);

    /**
     * This function will publish warnings about the reservoir water level to the mqtt
     * broker. Like messages of an low water level or an empty reservoir.
     *
     * @param warningType   The type of warning to be send.
     */
    void publishWarning( uint8_t warningType);

    /**
     * This function will start listening for configuration send by the mqtt broker.
     */
    void listenForConfiguration();

private:
    /**
      * This function will attempt to verify the TLS/SSL certificate send from the MQTT broker by its SHA1 fingerprint.
      * If the fingerprint doesn't match the one saved in the mqttInf1iGaFingerprint variable it will halt the execution
      * and print an error message to the serial port.
      */
    void verifyFingerprint();

    /**
     * This function callback will be subscribed to the plant care configuration topic.
     * When new plant care configuration gets published on this topic it will update the
     * pot's configuration.
     *
     * @param data      An json string containing plant care configuration.
     * @param length    The length of the json string.
     */
    static void listenForPlantCareConfiguration( char *data, uint16_t length );

    /**
    * This function callback will be subscribed to the mqtt configuration topic.
    * When new mqtt configuration gets published on this topic it will update the
    * pot's configuration.
    *
    * @param data      An json string containing mqtt configuration.
    * @param length    The length of the json string.
    */
    static void listenForMqttConfiguration(char *data, uint16_t length);

    /**
    * This function callback will be subscribed to the led configuration topic.
    * When new led configuration gets published on this topic it will update the
    * pot configuration.
    *
    * @param data      An json string containing led configuration.
    * @param length    The length of the json string.
    */
    static void listenForLedConfiguration(char *data, uint16_t length );
};

#endif //WATERUP_PLANTPOT_COMMUNICATION_H
