/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 17-06-2017 20:16
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Communication.h"

const char* potStatisticJsonFormat = "{\"mac\":\"%s\",\"type\":\"potstats-mesg\",\"counter\":%d,\"moisture\":%lu,\"waterLevel\":%d}";
const char* potWarningJsonFormat = "{\"mac\":\"%s\",\"type\":\"warning-mesg\",\"counter\":%d,\"warning\":\"%lu\"}";

String potMacAddress = "5C:CF:7F:19:9C:39"; // The mac addess of the plant pot.

char jsonMessageSendBuffer[JSON_BUFFER_SIZE]; // The buffer that will be filled with data to send to the MQTT broker.
char jsonMessageReceiveBuffer[JSON_BUFFER_SIZE]; // The buffer that will be filled with data received fro the MQTT broker.

uint32_t potStatisticCounter = 0; // An statistic message publication counter.
uint32_t potWarningCounter = 0; // An warning message publication counter.

WiFiClientSecure client;
Adafruit_MQTT_Client mqtt(&client, MQTT_BROKER_HOST, MQTT_BROKER_PORT, MQTT_BROKER_USERNAME, MQTT_BROKER_PASSWORD);

Adafruit_MQTT_Publish statisticPublisher = Adafruit_MQTT_Publish(&mqtt, MQTT_BROKER_USERNAME TOPIC_PUBLISH_STATISTIC);
Adafruit_MQTT_Publish warningPublisher = Adafruit_MQTT_Publish( &mqtt, MQTT_BROKER_USERNAME TOPIC_PUBLISH_WARNING );

Adafruit_MQTT_Subscribe configListener = Adafruit_MQTT_Subscribe(&mqtt, MQTT_BROKER_USERNAME TOPIC_SUBSCRIBE_PLANT_CONFIG);

void Communication::setup()
{
    Serial.begin(115200);
    delay(10);
    Serial << endl;

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
}

void Communication::connect()
{
    int8_t ret;
    uint8_t maxRetries = 10;

    if (mqtt.connected())
    {
        return;
    }

    Serial << F("[info] - Attempting to connect to the MQTT broker.") << endl;

    while ((ret = mqtt.connect())!=0)
    { // connect will return 0 for connected
        Serial << F("[error] - Connecting to the MQTT broker failed because: ") << mqtt.connectErrorString(ret)
               << endl; // Print an detailed error message.
        Serial << F("[info] - Retrying to connect to the MQTT broker in 5 seconds...") << endl;
        mqtt.disconnect(); // Send disconnect package.
        delay(1000);  // wait 5 seconds
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
        Serial
                << F("[error] - Connecting to the MQTT broker failed because the TLS/SSL certificate could not be verified.")
                << endl;
        Serial << F("[debug] - TLS/SSL SHA1 certificate fingerprint allowed: ") << MQTT_BROKER_FINGERPRINT << endl;
        Serial << F("[info] - Halting the execution of the program.") << endl;
        while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }
}

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

void Communication::publishWarning(WarningType warningType)
{
    snprintf(jsonMessageSendBuffer, JSON_BUFFER_SIZE, potWarningJsonFormat, WiFi.macAddress().c_str(), potWarningCounter++, (char*) warningType);
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

void Communication::startListenForConfiguration()
{
    //subscribeConfigTopic.setCallBack();
}

void Communication::plantCareConfigurationListener(char *data, uint16_t len)
{
    //todo implement save config.
}

void Communication::mqttConfigurationListener(char *data, uint16_t len)
{
    //todo implement save config.
}

void Communication::ledConfigurationListener(char *data, uint16_t len)
{
    //todo implement save config.
}