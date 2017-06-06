#ifndef MQTT_H
#define MQTT_H

#define wifiSSID "ASUS-ALWIN"
#define wifiPassword "test12345"
#define mqttBrokerHost "mqtt.inf1i.ga" // The address of the MQTT broker.
#define mqttBrokerPort 8883 // The port to connect to at the MQTT broker.
#define mqttBrokerUsername "inf1i-plantpot" // The pot's username authenticate at the MQTT broker.
#define mqttBrokerPassword "password" // The pot's password to authenticate at the MQTT broker.
#define publishStatisticsTopic "/publish/statistics" // This MQTT topic is used to publish pot state statistics.
#define publishWarningTopic "/publish/warning" // This MQTT topic is used to publish pot warnings such as empty water reservoir warnings.
#define subscribeConfigTopic "/subscribe/config" // This MQTT topic is used receive new pot configuration.
#define jsonBufferSize 200 // This holds the default string buffer size of json messages.

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "FS.h"
#include <Streaming.h>

class MQTT;

enum WarningType{
    LOW_RESORVOIR = 1,
    EMPTY_RESORVOIR = 2,
    LOW_MOISTURE_LEVEL = 3,
    HIGH_MOISTURE_LEVEL = 4,
    UNKNOWN_ERROR = 5
};

class MQTT{
    private:
        void verifyFingerprint();
    public:
        void MQTT_connect();
        void setup();
        void publish();
        void buildPotStatisticMessage(long groundMoistureLevel, int waterReservoirLevel);
        void buildPotStatisticMessage(WarningType warningType);
        void mqttConnect();
};

#endif