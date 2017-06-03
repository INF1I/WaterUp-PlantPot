/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "FS.h"

//### Defines for Flash saved Text #############################################
#define FPr(x);   Serial.print(FP(x));              // FlashPrint(FT)
#define FPrL(x);  Serial.println(FP(x));            // FlasPrintLine(FT)
#define SPFT(x);  Serial.print(FP(FT_##x));         // shorter version (ID)
#define SPFTL(x); Serial.println(FP(FT_##x));       // shorter version (ID)

#define SFP(x);   Serial.print(FP(x));

#define FT(x, y);  const char FT_##x[] PROGMEM = {y};// generate FlashText(ID,FT)
#define FP(x)     (__FlashStringHelper*)(x)         // Helper

//### Shortening standard Serial.Print F() #####################################
#define SPr(x);   Serial.print(x);                  // short for Serial.print
#define SPrL(x);  Serial.println(x);                // short for Serial.println
#define SPrF(x);  SPr(F(x));                        // short for F-Macro Serial.print
#define SPrLF(x); SPrL(F(x));                       // short for F-Macro Serial.println

#define wifISSID "iot-test"
#define wifiPassword "test12345"
#define mqttBrokerHost "mqtt.inf1i.ga"
#define mqttBrokerPort 8883
#define mqttBrokerUsername "inf1i-plantpot"
#define mqttBrokerPassword "password"

#define publishStatisticsTopic "/publish/statistics" // This MQTT topic is used to publish pot state statistics.
#define publishWarningTopic "/publish/warning" // This MQTT topic is used to publish pot warnings such as empty water reservoir warnings.
#define subscribeConfigTopic "/subscribe/config" // This MQTT topic is used receive new pot configuration.

#define jsonBufferSize 200 // This holds the default string buffer size of json messages.

enum WarningType{
    LOW_RESORVOIR = 1,
    EMPTY_RESORVOIR = 2,
    LOW_MOISTURE_LEVEL = 3,
    HIGH_MOISTURE_LEVEL = 4,
    UNKNOWN_ERROR = 5
};

/**
 * The SHA1 fingerprints taken from the backend server's SSL certificates.
 */
PROGMEM const char *apiInf1iGaFingerprint = "ED 1D 2C E2 41 5D 35 81 F6 15 DB A1 C8 0B 19 71 32 67 8B 46";
PROGMEM const char *mqttInf1iGaFingerprint = "A6 E4 A9 8C 92 B3 8D 81 73 CE 5B 33 33 F5 A3 7A 1B 87 E2 F3";

/**
 * Define JSON message templates that will be filled with data when send to an broker.
 */
PROGMEM char potStatisticJsonFormat[jsonBufferSize] = "{\"mac\":\"%s\",\"type\":\"potstats-mesg\",\"counter\":%zu,\"moisture\":%d,\"waterLevel\":%d}";
PROGMEM char potWarningJsonFormat[jsonBufferSize] = "{\"mac\":\"%s\",\"type\":\"warning-mesg\",\"counter\":%zu,\"warning\":\"%d\"}";

char jsonMessageSendBuffer[jsonBufferSize];
char jsonMessageReceiveBuffer[jsonBufferSize];

String potMacAddress = "";

uint32_t potStatisticCounter = 0; // An statistic message publication counter.
uint32_t potWarningCounter = 0; // An warning message publication counter.

/**
 * Setup the Wifi client for wireless communication to the internet that will be used to
 * transmit and receive messages from the broker. This wifi client has build in SSL/TLS support
 * so the data won't be transmitted over the internet in plain text.
 */
WiFiClientSecure wifiClient;

/**
 * Setup the MQTT client for communicating to the MQTT broker.
 */
Adafruit_MQTT_Client mqttClient(&wifiClient, mqttBrokerHost, mqttBrokerPort, mqttBrokerUsername, mqttBrokerPassword);

/**
 * Create the required publish and subscribe clients that will be used to communicate messages
 * between the pot, app and broker.
 */
Adafruit_MQTT_Publish statisticPublisher = Adafruit_MQTT_Publish(&mqttClient, mqttBrokerUsername publishStatisticsTopic);
Adafruit_MQTT_Publish warningPublisher = Adafruit_MQTT_Publish(&mqttClient, mqttBrokerUsername publishWarningTopic);
Adafruit_MQTT_Subscribe configSubscriber = Adafruit_MQTT_Subscribe(&mqttClient, mqttBrokerUsername subscribeConfigTopic);

/**
 * This function will programmatically reset the Arduino/Huzzah board.
 */
void (*resetFunc)(void) = 0;

/**
 * This function will connect to the MQTT broker if we aren't connected already. It will automatically try
 * to reconnect when the connection is lost.
 */
void mqttConnect();

/**
 * This function will attempt to verify the TLS/SSL certificate send from the MQTT broker by its SHA1 fingerprint.
 * If the fingerprint doesn't match the one saved in the mqttInf1iGaFingerprint variable it won't send anything and
 * print an error message to the serial port.
 */
void verifyFingerprint();

/**
 * Function for constructing an JSON pot statistic message.
 * @param groundMoistureLevel The measurement data from the soil humidity sensor.
 * @param waterReservoirLevel The measurement data from the ultra sonar sensor in the water reservoir.
 */
void buildPotStatisticMessage(word groundMoistureLevel, byte waterReservoirLevel);

/**
 * Function for constructing an JSON pot warning message.
 * @param warningType The type of warning that needs to be send.
 */
void buildPotStatisticMessage( WarningType warningType );

/**
 * This function is used to initiate the Arduino/Huzzah board. It gets executed whenever the board is
 * first powered up or after an rest.
 */
void setup()
{
    Serial.begin(115200); // Start serial communication for sending debug messages to the serial port.
    delay(10); // Fix to make connecting to the wifi network more stable.

    Serial.println(F("[info] - Setting up the plant pot."));
    Serial.print(F("[info] - Connecting to "));
    Serial.println(wifISSID);

    delay(1000); // Wait a second before connecting to the wifi network.
    WiFi.begin(wifISSID, wifiPassword); // Try to connect to the wifi network.
    delay(2000); // Wait 2 seconds before checking if the connection was opened successfully.

    while (WiFi.status() != WL_CONNECTED) // While we are not connected to the wifi network wait half a second.
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.println(F("[info] - Successfully connected to the wifi network."));
    Serial.println(F("[debug] - IP address assigned from the router: "));
    Serial.println(WiFi.localIP());
    Serial.print(F("[debug] - Plant pot mac address: "));

    verifyFingerprint(); // Check SHA1 fingerprint of the MQTT broker.
}

/**
 * This function will run as long as the board is powered on, it contains the main program code.
 */
void loop()
{
    mqttConnect(); // Connect to the MQTT broker if we aren't connected already. If it fails it will retry until the end of times.

    buildPotStatisticMessage(1024, 255); // Build an pot statistic JSON message and save it to the JSON send buffer.
    Serial.print(F("\n[debug] - Publishing message to the broker: "));
    Serial.println(jsonMessageSendBuffer);

    if (!statisticPublisher.publish(jsonMessageSendBuffer)) // Did we publish the message to the broker?
    {
        Serial.print(F("[error] - Unable to send message: "));
        Serial.println(jsonMessageSendBuffer);
    }
    else
    {
        Serial.print(F("[debug] - Message with id: "));
        Serial.print(potStatisticCounter);
        Serial.println(F("[info] - Successfully published message to the MQTT broker."));
    }

    delay(2000); // Wait 2 seconds before spamming the broker again.
}

/**
 * This function will attempt to verify the TLS/SSL certificate send from the MQTT broker by its SHA1 fingerprint.
 * If the fingerprint doesn't match the one saved in the mqttInf1iGaFingerprint variable it will halt the execution
 * and print an error message to the serial port.
 */
void verifyFingerprint()
{
    const char *host = mqttBrokerHost;

    Serial.print(F("[info] - Attempting to open an secure connection to the MQTT broker at:"));
    Serial.println(host);

    if (!wifiClient.connect(host, mqttBrokerPort))
    {
        Serial.println(F("[error] - Connecting to the MQTT broker failed because we can't reach it."));
        Serial.println(F("[info] - Halting the execution of the program."));
        while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }

    if (wifiClient.verify(mqttInf1iGaFingerprint, host))
    {
        Serial.println("[info] - Successfully verified the integrity of the TLS/SSL certificate send by the broker.");
    }
    else
    {
        Serial.println(F("[error] - Connecting to the MQTT broker failed because the TLS/SSL certificate could not be verified."));
        Serial.print(F("[debug] - TLS/SSL SHA1 certificate fingerprint allowed: "));
        Serial.println(mqttInf1iGaFingerprint);
        Serial.println(F("[info] - Halting the execution of the program."));
        while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }

}

/**
 * This function will connect to the MQTT broker if we aren't connected already. It will automatically try
 * to reconnect when the connection is lost.
 */
void mqttConnect()
{
    int8_t ret;
    uint8_t maxRetries = 10;

    if (mqttClient.connected()) // Are we already connected? If so continue caring for the plant.
    {
        return;
    }

    Serial.println(F("[info] - Attempting to connect to the MQTT broker."));

    while ((ret = mqttClient.connect()) != 0) // While we are not connected
    {
        Serial.print("[error] - Connecting to the MQTT broker failed because: ");
        Serial.println(mqttClient.connectErrorString(ret)); // Print an detailed error message.
        Serial.println("[info] - Retrying to connect to the MQTT broker in 5 seconds...");
        mqttClient.disconnect(); // Send disconnect package.

        delay(5000);  // Wait 5 seconds before attempting to reconnect to the MQTT broker.
        maxRetries--; // Connecting failed so subtract one retry attempt.

        if (maxRetries == 0) // It seems to be impossible to connect to the MQTT broker so halt the execution of the program.
        {
            Serial.println(F("[error] - Connecting to the MQTT broker failed it seems the broker is unavailable."));
            Serial.println(F("[info] - Halting the execution of the program."));
            while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
            {
            }
        }
    }
    /**
     * Everything went fine we are now connected to the MQTT broker.
     */
    Serial.println("[info] - Successfully connected to the MQTT broker.");
}

/**
 * Function for constructing an JSON pot statistic message.
 * @param groundMoistureLevel The measurement data from the soil humidity sensor.
 * @param waterReservoirLevel The measurement data from the ultra sonar sensor in the water reservoir.
 */
void buildPotStatisticMessage(word groundMoistureLevel, byte waterReservoirLevel)
{
    // Create the JSON message based on the specified format and write it to the send buffer.
    snprintf(jsonMessageSendBuffer, jsonBufferSize, potStatisticJsonFormat, potMacAddress, potStatisticCounter++, groundMoistureLevel, waterReservoirLevel);
}

/**
 * Function for constructing an JSON pot warning message.
 * @param warningType The type of warning that needs to be send.
 */
void buildPotStatisticMessage( WarningType warningType )
{
    // Create the JSON message based on the specified format and write it to the send buffer.
    snprintf(jsonMessageSendBuffer, jsonBufferSize, potWarningJsonFormat, potMacAddress, potWarningCounter++, warningType);
}
