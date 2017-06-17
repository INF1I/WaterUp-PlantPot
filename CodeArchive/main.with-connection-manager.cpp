#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <Adafruit_MQTT.h> // Include this library for securely connecting to the internet using WiFi.
#include <Adafruit_MQTT_Client.h> // Include this library for MQTT communication.
#include <FS.h> // Include this library for access to the ESP8266's file system.
#include <Streaming.h> // Include this library for using the << Streaming operator.

#define MQTT_BROKER_HOST "mqtt.inf1i.ga" // The address of the MQTT broker.
#define mqttBrokerPort 8883 // The port to connect to at the MQTT broker.
#define mqttBrokerUsername "inf1i-plantpot" // The pot's username authenticate at the MQTT broker.
#define mqttBrokerPassword "password" // The pot's password to authenticate at the MQTT broker.
#define publishStatisticsTopic "/publish/statistics" // This MQTT topic is used to publish pot state statistics.
#define jsonBufferSize 200 // This holds the default string buffer size of json messages.

unsigned long previousMillisStatistics = 0; // The last milliseconds an pot statistic was published to the broker.
unsigned long statisticMillisInterval = 10000; // The interval for publishing pot statistics to the broker.

const char *mqttInf1iGaFingerprint = "A6 E4 A9 8C 92 B3 8D 81 73 CE 5B 33 33 F5 A3 7A 1B 87 E2 F3";
const char *potStatisticJsonFormat = "{\"mac\":\"%s\",\"type\":\"potstats-mesg\",\"counter\":%d,\"moisture\":%lu,\"waterLevel\":%d}";

String potMacAddress = "5C:CF:7F:19:9C:39"; // The mac addess of the plant pot.

char jsonMessageSendBuffer[jsonBufferSize]; // The buffer that will be filled with data to send to the MQTT broker.

uint32_t potStatisticCounter = 0; // An statistic message publication counter.

WiFiClientSecure client;
Adafruit_MQTT_Client mqtt(&client, MQTT_BROKER_HOST, mqttBrokerPort, mqttBrokerUsername, mqttBrokerPassword);
Adafruit_MQTT_Publish statisticPublisher = Adafruit_MQTT_Publish(&mqtt, mqttBrokerUsername publishStatisticsTopic);

void verifyFingerprint();
void mqttConnect();
void publishPotStatistic();
void buildPotStatisticMessage(long groundMoistureLevel, int waterReservoirLevel);

void setup()
{
    previousMillisStatistics = millis();
    Serial.begin(115200);
    delay(10);
    Serial.println("");

    WiFi.printDiag(Serial);

    WiFiManager wifiManager;
    wifiManager.autoConnect();

    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));
    }

    Serial << F("[info] - Successfully connected to the wifi network.") << endl;
    Serial << F("[debug] - IP address assigned from the router: ") << WiFi.localIP() << endl;
    Serial << F("[info] - Successfully connected to the wifi network.") << endl;
    Serial << F("[debug] - Plant pot mac address: ") << WiFi.macAddress() << endl;
    verifyFingerprint(); // Check SHA1 fingerprint of the MQTT broker.
    potMacAddress = WiFi.macAddress();
}

void loop()
{
    mqttConnect();

    unsigned long currentMillis = millis();

    if( currentMillis - previousMillisStatistics == statisticMillisInterval )
    {
        Serial << F("Topic:") << mqttBrokerUsername publishStatisticsTopic << endl;
        Serial << F("Message:") << jsonMessageSendBuffer << endl;
        previousMillisStatistics = currentMillis;
        publishPotStatistic();
    }
}

void mqttConnect() {
    int8_t ret;
    uint8_t maxRetries = 10;

    if (mqtt.connected())
    {
        return;
    }

    Serial << F("[info] - Attempting to connect to the MQTT broker.") << endl;

    while ((ret = mqtt.connect()) != 0)
    { // connect will return 0 for connected
        Serial << F("[error] - Connecting to the MQTT broker failed because: ") << mqtt.connectErrorString(ret) << endl; // Print an detailed error message.
        Serial << F("[info] - Retrying to connect to the MQTT broker in 5 seconds...") << endl;
        mqtt.disconnect(); // Send disconnect package.
        delay(1000);  // wait 5 seconds
        maxRetries--;

        if( maxRetries == 0 )
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

void verifyFingerprint()
{
    const char *host = MQTT_BROKER_HOST;
    Serial << F("[info] - Attempting to open an secure connection to the MQTT broker at:") << host << endl;

    if (!client.connect(host, mqttBrokerPort))
    {
        Serial << F("[error] - Connecting to the MQTT broker failed because we can't reach it.") << endl;
        Serial << F("[info] - Halting the execution of the program.") << endl;
        while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }

    if (client.verify(mqttInf1iGaFingerprint, host))
    {
        Serial << "[info] - Successfully verified the integrity of the TLS/SSL certificate send by the broker." << endl;
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

void publishPotStatistic()
{
    buildPotStatisticMessage( 10, 10);

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

void buildPotStatisticMessage(long groundMoistureLevel, int waterReservoirLevel)
{
    snprintf(jsonMessageSendBuffer, jsonBufferSize, potStatisticJsonFormat, potMacAddress.c_str(), potStatisticCounter++, groundMoistureLevel, waterReservoirLevel);
}