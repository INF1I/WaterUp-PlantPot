#ifndef MQTT_H
#define MQTT_H

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
        void buildPotStatisticMessage(uint16_t groundMoistureLevel, uint8_t waterReservoirLevel);
        void buildPotStatisticMessage( WarningType warningType );
        void mqttConnect();
};

#endif