/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "MQTT.h"

/**
 * The SHA1 fingerprints taken from the backend server's SSL certificates.
 */
const char *apiInf1iGaFingerprint = "ED 1D 2C E2 41 5D 35 81 F6 15 DB A1 C8 0B 19 71 32 67 8B 46";
const char *mqttInf1iGaFingerprint = "A6 E4 A9 8C 92 B3 8D 81 73 CE 5B 33 33 F5 A3 7A 1B 87 E2 F3";

/**
 * Define JSON message templates that will be filled with data when send to an broker.
 */
const char *potStatisticJsonFormat = "{\"mac\":\"%s\",\"type\":\"potstats-mesg\",\"counter\":%d,\"moisture\":%lu,\"waterLevel\":%d}";
const char *potWarningJsonFormat = "{\"mac\":\"%s\",\"type\":\"warning-mesg\",\"counter\":%d,\"warning\":\"%lu\"}";

char jsonMessageSendBuffer[jsonBufferSize]; // The buffer that will be filled with data to send to the MQTT broker.
char jsonMessageReceiveBuffer[jsonBufferSize]; // The buffer that will be filled with data received fro the MQTT broker.

const char *potMacAddress = "5C:CF:7F:19:9C:39"; // The mac addess of the plant pot.

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
 * This function is used to initiate the Arduino/Huzzah board. It gets executed whenever the board is
 * first powered up or after an rest.
 */
void MQTT::setup(Sensors *sensorsLib)
{
    sensorsLibrary = sensorsLib;
    sensorsLibrary->setup();
    Serial.begin(115200); // Start serial communication for sending debug messages to the serial port.
    delay(10); // Fix to make connecting to the wifi network more stable.

#if( MQTT_DEBUG_MODE > 0 )
    Serial << F("[info] - Setting up the plant pot.") << endl;
    Serial << F("[info] - Connecting to ") << wifiSSID << endl;
#endif

    delay(1000); // Wait a second before connecting to the wifi network.
    WiFi.begin(wifiSSID, wifiPassword); // Try to connect to the wifi network.
    delay(2000); // Wait 2 seconds before checking if the connection was opened successfully.

    while (WiFi.status() != WL_CONNECTED) // While we are not connected to the wifi network wait half a second.
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

#if( MQTT_DEBUG_MODE > 0)
    Serial << F("[info] - Successfully connected to the wifi network.") << endl;
    Serial << F("[debug] - IP address assigned from the router: ") << WiFi.localIP() << endl;
    Serial << F("[info] - Successfully connected to the wifi network.") << endl;
    Serial << F("[debug] - Plant pot mac address: ") << WiFi.macAddress() << endl;
#endif
    verifyFingerprint(); // Check SHA1 fingerprint of the MQTT broker.
}

/**
 * This function will attempt to verify the TLS/SSL certificate send from the MQTT broker by its SHA1 fingerprint.
 * If the fingerprint doesn't match the one saved in the mqttInf1iGaFingerprint variable it will halt the execution
 * and print an error message to the serial port.
 */
void MQTT::verifyFingerprint()
{
    const char *host = mqttBrokerHost;

#if( MQTT_DEBUG_MODE > 0)
    Serial << F("[info] - Attempting to open an secure connection to the MQTT broker at:") << host << endl;
#endif
    if (!wifiClient.connect(host, mqttBrokerPort))
    {
        Serial << F("[error] - Connecting to the MQTT broker failed because we can't reach it.") << endl;
        Serial << F("[info] - Halting the execution of the program.") << endl;
        while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }

    if (wifiClient.verify(mqttInf1iGaFingerprint, host))
    {
#if( MQTT_DEBUG_MODE > 0 )
        Serial << "[info] - Successfully verified the integrity of the TLS/SSL certificate send by the broker." << endl;
#endif
    }
    else
    {
        Serial << F("[error] - Connecting to the MQTT broker failed because the TLS/SSL certificate could not be verified.") << endl;
        Serial << F("[debug] - TLS/SSL SHA1 certificate fingerprint allowed: ") << mqttInf1iGaFingerprint << endl;
        Serial << F("[info] - Halting the execution of the program.") << endl;
        while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }
}

/**
 * This function will connect to the MQTT broker if we aren't connected already. It will automatically try
 * to reconnect when the connection is lost.
 */
void MQTT::mqttConnect()
{
    int8_t ret;
    uint8_t maxRetries = 10;

    if (mqttClient.connected()) // Are we already connected? If so continue caring for the plant.
    {
        return;
    }

#if( MQTT_DEBUG_MODE > 0 )
    Serial << F("[info] - Attempting to connect to the MQTT broker.") << endl;
#endif
    while ((ret = mqttClient.connect()) != 0) // While we are not connected
    {
        Serial << F("[error] - Connecting to the MQTT broker failed because: ") << mqttClient.connectErrorString(ret) << endl; // Print an detailed error message.
        Serial << F("[info] - Retrying to connect to the MQTT broker in 5 seconds...") << endl;
        mqttClient.disconnect(); // Send disconnect package.

        delay(5000);  // Wait 5 seconds before attempting to reconnect to the MQTT broker.
        maxRetries--; // Connecting failed so subtract one retry attempt.

        if (maxRetries == 0) // It seems to be impossible to connect to the MQTT broker so halt the execution of the program.
        {
            Serial << F("[error] - Connecting to the MQTT broker failed it seems the broker is unavailable.") << endl;
            Serial << F("[info] - Halting the execution of the program.") << endl;
            while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
            {
            }
        }
    }
#if( MQTT_DEBUG_MODE > 0 )
    /**
     * Everything went fine we are now connected to the MQTT broker.
     */
    Serial << "[info] - Successfully connected to the MQTT broker." << endl;
#endif
}

/**
 * This function is used to publish statistics about the pot's state to the broker.
 */
void MQTT::publishPotStatistic()
{
    buildPotStatisticMessage((long) sensorsLibrary->getMoistureLevel(), (int) sensorsLibrary->calcWaterLevel());

    if (!statisticPublisher.publish(jsonMessageSendBuffer)) // Did we publish the message to the broker?
    {
        Serial << F("[error] - Unable to send message: ") << jsonMessageSendBuffer << endl;
    }
    else
    {
#if( MQTT_DEBUG_MODE > 0 )
        Serial << F("[debug] - Message with id: ") << potStatisticCounter << F(" content: ") << jsonMessageSendBuffer << endl;
        Serial << F("[info] - Successfully published message to the MQTT broker.") << endl;
#endif
    }
}

/**
 * This function will publish pot statistic messages and warnings.
 */
void MQTT::runLoop()
{
    unsigned long currentMillis = millis();

    if( currentMillis - previousMillisStatistics == statisticMillisInterval )
    {
        previousMillisStatistics = currentMillis;
        publishPotStatistic();
    }
}


/**
 * Function for constructing an JSON pot statistic message.
 * @param groundMoistureLevel The measurement data from the soil humidity sensor.
 * @param waterReservoirLevel The measurement data from the ultra sonar sensor in the water reservoir.
 */
void MQTT::buildPotStatisticMessage(long groundMoistureLevel, int waterReservoirLevel)
{
    // Create the JSON message based on the specified format and write it to the send buffer.
    snprintf(jsonMessageSendBuffer, jsonBufferSize, potStatisticJsonFormat, potMacAddress, potStatisticCounter++, groundMoistureLevel, waterReservoirLevel);
}

/**
 * Function for constructing an JSON pot warning message.
 * @param warningType The type of warning that needs to be send.
 */
void MQTT::buildPotStatisticMessage(WarningType warningType)
{
    // Create the JSON message based on the specified format and write it to the send buffer.
    snprintf(jsonMessageSendBuffer, jsonBufferSize, potWarningJsonFormat, potMacAddress, potWarningCounter++, (char *) warningType);
}