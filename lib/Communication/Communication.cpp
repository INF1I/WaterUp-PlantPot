/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Created: 17-06-2017 20:16
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Communication.h"

/**
 * The json string C-style formatted that will be filled with data and send to the mqtt broker.
 */
const char* potStatisticJsonFormat = "{\"mac\":\"%s\",\"type\":\"potstats-mesg\",\"counter\":%d,\"moisture\":%lu,\"waterLevel\":%d}";

/**
 * The json string C-style formatted that will be filled with data and send to the mqtt broker.
 */
const char* potWarningJsonFormat = "{\"mac\":\"%s\",\"type\":\"warning-mesg\",\"counter\":%d,\"warning\":\"%lu\"}";

/**
 * The mac address of the plant pot. It is set to an default but will be overwritten after the
 * constructor is called.
 */
String potMacAddress = "5C:CF:7F:19:9C:39"; // The mac addess of the plant pot.

char jsonMessageSendBuffer[JSON_BUFFER_SIZE]; // The buffer that will be filled with data to send to the MQTT broker.
char jsonMessageReceiveBuffer[JSON_BUFFER_SIZE]; // The buffer that will be filled with data received fro the MQTT broker.

uint32_t potStatisticCounter = 0; // An statistic message publication counter.
uint32_t potWarningCounter = 0; // An warning message publication counter.

/**
 * Setup the Wifi client for wireless communication to the internet that will be used to
 * transmit and receive messages from the broker. This wifi client has build in SSL/TLS support
 * so the data won't be transmitted over the internet in plain text.
 */
WiFiClientSecure client;

/**
 * Setup the MQTT client for communicating to the MQTT broker.
 */
Adafruit_MQTT_Client mqtt(&client, MQTT_BROKER_HOST, MQTT_BROKER_PORT, MQTT_BROKER_USERNAME, MQTT_BROKER_PASSWORD);

/**
 * Create the required publish clients that will be used to send messages to the mqtt broker
 * that will send it to the end users.
 */
Adafruit_MQTT_Publish statisticPublisher = Adafruit_MQTT_Publish(&mqtt, MQTT_BROKER_USERNAME TOPIC_PUBLISH_STATISTIC);
Adafruit_MQTT_Publish warningPublisher = Adafruit_MQTT_Publish( &mqtt, MQTT_BROKER_USERNAME TOPIC_PUBLISH_WARNING );

/**
 * Create the required subscribe clients that listen for incoming configuration messages send by the
 * mqtt broker.
 */
Adafruit_MQTT_Subscribe ledConfigListener = Adafruit_MQTT_Subscribe(&mqtt, TOPIC_SUBSCRIBE_LED_CONFIG);
Adafruit_MQTT_Subscribe mqttConfigListener = Adafruit_MQTT_Subscribe(&mqtt, TOPIC_SUBSCRIBE_MQTT_CONFIG);
Adafruit_MQTT_Subscribe plantCareConfigListener = Adafruit_MQTT_Subscribe(&mqtt, TOPIC_SUBSCRIBE_PLANT_CARE_CONFIG);

/*
void listenForPlantCareConfiguration(char *data, uint16_t len);
void listenForMqttConfiguration(char *data, uint16_t len);
void listenForLedConfiguration(char *data, uint16_t len);
*/

/**
 * The constructor will initiate the communication library with some default
 * values and will save an reference to the configuration library.
 * @param potConfiguration  An pointer to the configuration library.
 */
Communication::Communication( Configuration * potConfiguration )
{
    Communication::potConfig = potConfiguration;
}

/**
 * This function initiates the communication settings. It will try to connect to the last
 * configured wifi network it it fails it will create an access point hosts an configuration
 * website where an user can connect to and set the wifi configuration.
 */
void Communication::setup()
{
    delay(10);
    Serial << endl;
    Serial << "[debug] - Setting up the communication library" << endl;

    WiFi.printDiag(Serial);

    WiFiManager wifiManager;
    wifiManager.autoConnect();

    while (WiFi.waitForConnectResult()!=WL_CONNECTED)
    {
        delay(500);
        Serial << F(".");
    }

    Serial << F("[info] - Successfully connected to the wifi network.") << endl;
    Serial << F("[debug] - IP address assigned from the router: ") << WiFi.localIP() << endl;
    Serial << F("[info] - Successfully connected to the wifi network.") << endl;
    Serial << F("[debug] - Plant pot mac address: ") << WiFi.macAddress() << endl;
    verifyFingerprint(); // Check SHA1 fingerprint of the MQTT broker.
    potMacAddress = WiFi.macAddress();
    //this->listenForConfiguration();// Subscribe mqtt configuration listeners.
}

/**
 * This function checks if there already is an mqtt connection if not it will attempt to open one. If
 * it fails to manny times it will kill its self so the user has to reset the plant pot.
 */
void Communication::connect()
{
    int8_t ret;
    uint8_t maxRetries = 50;

    if (mqtt.connected())
    {
        return;
    }

    Serial << F("[info] - Attempting to connect to the MQTT broker.") << endl;

    while ((ret = mqtt.connect())!=0)
    { // connect will return 0 for connected
        Serial << F("[error] - Connecting to the MQTT broker failed because: ") << mqtt.connectErrorString(ret) << endl; // Print an detailed error message.
        Serial << F("[info] - Retrying to connect to the MQTT broker in 5 seconds...") << endl;

        mqtt.disconnect(); // Send disconnect package.
        delay(2000);  // wait 5 seconds
        maxRetries--;

        if (maxRetries==0)
        {
            Serial << F("[error] - Connecting to the MQTT broker failed it seems the broker is unavailable.") << endl;
            Serial << F("[info] - Halting the execution of the program.") << endl;
            while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
            {
            }
        }
    }

    Serial << "[info] - Successfully connected to the MQTT broker." << endl;
}

/**
 * This function will return the pointer to the configuration object that
 * contains communication and plant care settings.
 *
 * @return *Configuration
 */
Configuration* Communication::getConfiguration()
{
    return Communication::potConfig;
}

/**
 * This function will publish statistics about the pot's current state to the
 * mqtt broker. It will fill json send buffer with the the C-style formatted
 * json whrere the placeholders are replaced with the correct data and pass the
 * buffer to the mqtt publisher.
 *
 * @param groundMoistureLevel   The current percentage of moisture in the ground.
 * @param waterReservoirLevel   The current percentage of water left in the reservoir.
 */
void Communication::publishStatistic(int groundMoistureLevel, int waterReservoirLevel)
{
    snprintf(jsonMessageSendBuffer, JSON_BUFFER_SIZE, potStatisticJsonFormat, WiFi.macAddress().c_str(), potStatisticCounter++, groundMoistureLevel, waterReservoirLevel);
    if (!statisticPublisher.publish(jsonMessageSendBuffer)) // Did we publish the message to the broker?
    {
        Serial << F("[error] - Unable to send message: ") << jsonMessageSendBuffer << endl;
    }
    else
    {
        Serial << F("[debug] - Message with id: ") << potStatisticCounter << F(" content: ") << jsonMessageSendBuffer << endl;
        Serial << F("[info] - Successfully published message to the MQTT broker.") << endl;
        Serial << F("[debug] - Publish topic: ") << MQTT_BROKER_USERNAME << TOPIC_PUBLISH_STATISTIC;
    }
}

/**
 * This function will publish warnings about the reservoir water level to the mqtt
 * broker. It will fill json send buffer with the the C-style formatted
 * json whrere the placeholders are replaced with the correct data and pass the
 * buffer to the mqtt publisher.
 *
 * @param warningType   The type of warning to be send.
 */
void Communication::publishWarning( uint8_t warningType)
{
    snprintf(jsonMessageSendBuffer, JSON_BUFFER_SIZE, potWarningJsonFormat, WiFi.macAddress().c_str(), potWarningCounter++, warningType);
    if (!statisticPublisher.publish(jsonMessageSendBuffer)) // Did we publish the message to the broker?
    {
        Serial << F("[error] - Unable to send message: ") << jsonMessageSendBuffer << endl;
    }
    else
    {
        Serial << F("[debug] - Message with id: ") << potStatisticCounter << F(" content: ") << jsonMessageSendBuffer << endl;
        Serial << F("[info] - Successfully published message to the MQTT broker.") << endl;
    }
}

/**
 * This function will start listening for configuration send by the mqtt broker. It will register
 * the callback functions to the mqtt listeners so when an message is received it knows what function
 * to execute.
 */
void Communication::listenForConfiguration()
{
    ledConfigListener.setCallback( Communication::listenForLedConfiguration );
    mqttConfigListener.setCallback( Communication::listenForMqttConfiguration );
    plantCareConfigListener.setCallback( Communication::listenForPlantCareConfiguration );

    mqtt.subscribe(&ledConfigListener);
    mqtt.subscribe(&mqttConfigListener);
    mqtt.subscribe(&plantCareConfigListener);
}

/**
 * This function will attempt to verify the TLS/SSL certificate send from the MQTT broker by its SHA1 fingerprint.
 * We use the SHA1 fingerprints instead of the complete certificates because of the memory limitations
 * of the arduino/huzzah. It will get the SHA1 send by the mqtt broker and compare it with the fingerprint hard
 * coded in the code stored on the plant pot.
 */
void Communication::verifyFingerprint()
{
    Serial << F("[info] - Attempting to open an secure connection to the MQTT broker at:") << MQTT_BROKER_HOST << endl;

    if (!client.connect(MQTT_BROKER_HOST, MQTT_BROKER_PORT))
    {
        Serial << F("[error] - Connecting to the MQTT broker failed because we can't reach it.") << endl;
        Serial << F("[info] - Halting the execution of the program.") << endl;
        while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }

    if (client.verify(MQTT_BROKER_FINGERPRINT, MQTT_BROKER_HOST))
    {
        Serial << "[info] - Successfully verified the integrity of the TLS/SSL certificate send by the broker." << endl;
    }
    else
    {
        Serial << F("[error] - Connecting to the MQTT broker failed because the TLS/SSL certificate could not be verified.") << endl;
        Serial << F("[debug] - TLS/SSL SHA1 certificate fingerprint allowed: ") << MQTT_BROKER_FINGERPRINT << endl;
        Serial << F("[info] - Halting the execution of the program.") << endl;
        while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }
}

/**
 * This function callback will be subscribed to the plant care configuration topic.
 * When new plant care configuration gets published on this topic it will update the
 * pot's configuration.
 *
 * @param data      An json string containing plant care configuration.
 * @param length    The length of the json string.
 */
void Communication::listenForLedConfiguration(char *data, uint16_t len)
{
    /*Serial << F("[debug] - Incoming led configuration message.") << endl << data << endl;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(data);

    const char* mac = root["mac"];
    const char* type = root["type"];
    long optimalMoisture = root["moistureneed"];
    long measurementInterval = root["interval"];

    if( strcmp( potMacAddress.c_str(), mac))
    {
        Serial << F("[debug] - The mac address is valid") << endl;
    }
    else
    {
        Serial << F("[debug] - The mac address is not valid") << endl;
    }*/
    //todo parse json
    //todo check if correct mac address.
    //todo insert received config into settings
    //Communication::potConfig->setLedSettings();
}

/**
 * This function callback will be subscribed to the mqtt configuration topic.
 * When new mqtt configuration gets published on this topic it will update the
 * pot's configuration.
 *
 * @param data      An json string containing mqtt configuration.
 * @param length    The length of the json string.
 */
void Communication::listenForMqttConfiguration(char *data, uint16_t len)
{
    /*Serial << F("[debug] - Incoming mqtt configuration message.") << endl << data << endl;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(data);

    const char* mac = root["mac"];
    const char* type = root["type"];
    uint8_t optimalMoisture = root["red"];
    uint8_t optimalMoisture = root["green"];
    uint8_t optimalMoisture = root["blue"];

    if( strcmp( potMacAddress.c_str(), mac))
    {
        Serial << F("[debug] - The mac address is valid") << endl;
    }
    else
    {
        Serial << F("[debug] - The mac address is not valid") << endl;
    }*/
    //todo parse json
    //todo check if correct mac address.
    //todo insert received config into settings
    //Communication::potConfig->setLedSettings();
}

/**
  * This function callback will be subscribed to the led configuration topic.
  * When new led configuration gets published on this topic it will update the
  * pot configuration.
  *
  * @param data      An json string containing led configuration.
  * @param length    The length of the json string.
  */
void Communication::listenForPlantCareConfiguration(char *data, uint16_t len)
{
   /* Serial << F("[debug] - Incoming plant care configuration message.") << endl << data << endl;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(data);

    const char* mac = root["mac"];
    const char* type = root["type"];
    long optimalMoisture = root["stat-interval"];
    long optimalMoisture = root["resend-interval"];
    long optimalMoisture = root["ping-interval"];
    uint8_t optimalMoisture = root["publish-threshold"];

    if( strcmp( potMacAddress.c_str(), mac))
    {
        Serial << F("[debug] - The mac address is valid") << endl;
    }
    else
    {
        Serial << F("[debug] - The mac address is not valid") << endl;
    }*/
    //todo parse json
    //todo check if correct mac address.
    //todo insert received config into settings
    //Communication::potConfig->setLedSettings();
}


