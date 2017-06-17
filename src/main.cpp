#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <Adafruit_MQTT.h> // Include this library for securely connecting to the internet using WiFi.
#include <Adafruit_MQTT_Client.h> // Include this library for MQTT communication.
#include <FS.h> // Include this library for access to the ESP8266's file system.
#include <Streaming.h> // Include this library for using the << Streaming operator.

#define mqttBrokerHost "mqtt.inf1i.ga" // The address of the MQTT broker.
#define mqttBrokerPort 8883 // The port to connect to at the MQTT broker.
#define mqttBrokerUsername "inf1i-plantpot" // The pot's username authenticate at the MQTT broker.
#define mqttBrokerPassword "password" // The pot's password to authenticate at the MQTT broker.

#define publishStatisticsTopic "/publish/statistics" // This MQTT topic is used to publish pot state statistics.
#define publishWarningTopic "/publish/warning" // This MQTT topic is used to publish pot warnings such as empty water reservoir warnings.
#define subscribeConfigTopic "/subscribe/config" // This MQTT topic is used receive new pot configuration.

#define jsonBufferSize 200 // This holds the default string buffer size of json messages.

#define potHeight 40 // Height what the water can reach
#define waterReservoirSurfaceSize 500 // The cm2 surface of the water reservoir.
#define waterReservoirSize 20000 // The cm3 content of the water reservoir.
#define trigPin 13 // The pin connected trigger port of the ultra sonar sensor.
#define echoPin 12 // The pin connected to the echo port of the ultra sonar sensor.
#define soilSensePin A0 // The pin connected to the analog read of the soil moisture sensor.
#define waterPumpPin 16 // The pin connected to the transistor base for switching the water pump.

enum WarningType
{
    LOW_RESORVOIR = 1,
    EMPTY_RESORVOIR = 2,
    LOW_MOISTURE_LEVEL = 3,
    HIGH_MOISTURE_LEVEL = 4,
    UNKNOWN_ERROR = 5
};

unsigned long previousMillisStatistics; // The last milliseconds an pot statistic was published to the broker.
unsigned long statisticMillisInterval = 10000; // The interval for publishing pot statistics to the broker.
unsigned long previousWaterPumpMillis;

const char *apiInf1iGaFingerprint = "ED 1D 2C E2 41 5D 35 81 F6 15 DB A1 C8 0B 19 71 32 67 8B 46";
const char *mqttInf1iGaFingerprint = "A6 E4 A9 8C 92 B3 8D 81 73 CE 5B 33 33 F5 A3 7A 1B 87 E2 F3";
const char *potStatisticJsonFormat = "{\"mac\":\"%s\",\"type\":\"potstats-mesg\",\"counter\":%d,\"moisture\":%lu,\"waterLevel\":%d}";
const char *potWarningJsonFormat = "{\"mac\":\"%s\",\"type\":\"warning-mesg\",\"counter\":%d,\"warning\":\"%lu\"}";
const char *potMacAddress = "5C:CF:7F:19:9C:39"; // The mac addess of the plant pot.

char jsonMessageSendBuffer[jsonBufferSize]; // The buffer that will be filled with data to send to the MQTT broker.
char jsonMessageReceiveBuffer[jsonBufferSize]; // The buffer that will be filled with data received fro the MQTT broker.

uint32_t potStatisticCounter = 0; // An statistic message publication counter.
uint32_t potWarningCounter = 0; // An warning message publication counter.

WiFiClientSecure client;
Adafruit_MQTT_Client mqtt(&client, mqttBrokerHost, mqttBrokerPort, mqttBrokerUsername, mqttBrokerPassword);
Adafruit_MQTT_Publish statisticPublisher = Adafruit_MQTT_Publish(&mqtt, mqttBrokerUsername publishStatisticsTopic);
Adafruit_MQTT_Publish warningPublisher = Adafruit_MQTT_Publish(&mqtt, mqttBrokerUsername publishWarningTopic);
Adafruit_MQTT_Subscribe configSubscriber = Adafruit_MQTT_Subscribe(&mqtt, mqttBrokerUsername subscribeConfigTopic);

void verifyFingerprint();
void mqttConnect();
void publishPotStatistic();
void buildPotStatisticMessage(long groundMoistureLevel, int waterReservoirLevel);
void buildPotStatisticMessage(WarningType warningType);
long getDistance();
int getMoistureLevel();
int calcWaterLevel();
void activateWaterPump(unsigned long duration );
void activateWaterPump();
void deactivateWaterPump();

void setup()
{
    pinMode( trigPin, OUTPUT );
    pinMode( echoPin, OUTPUT );
    pinMode( soilSensePin, INPUT );
    pinMode( waterPumpPin, OUTPUT );
    digitalWrite( waterPumpPin, LOW );
    Serial.begin(115200);
    delay(10);
    Serial.println("");
    Serial.print(F("\nAdafruit IO anonymous Time Demo"));

    WiFi.printDiag(Serial);

    WiFiManager wifiManager;
    wifiManager.autoConnect();

    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }
    potMacAddress = WiFi.macAddress().c_str();
    Serial << F("[info] - Successfully connected to the wifi network.") << endl;
    Serial << F("[debug] - IP address assigned from the router: ") << WiFi.localIP() << endl;
    Serial << F("[info] - Successfully connected to the wifi network.") << endl;
    Serial << F("[debug] - Plant pot mac address: ") << WiFi.macAddress() << endl;
    verifyFingerprint(); // Check SHA1 fingerprint of the MQTT broker.
}

void loop()
{
    mqttConnect();

    unsigned long currentMillis = millis();

    if( currentMillis - previousMillisStatistics == statisticMillisInterval )
    {
        previousMillisStatistics = currentMillis;
        publishPotStatistic();
    }
}
/*******************************************************************************************************SENSORS
 * Sensors
 */
long getDistance()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long Duration = pulseIn(echoPin, HIGH); //Listening and waiting for wave
    delay(10);
    return (Duration * 0.034 / 2);//Converting the reported number to CM
}
int getMoistureLevel()
{
    int v = analogRead(soilSensePin);
    return v;
}
int calcWaterLevel()
{
    float distance = getDistance();
    float waterContent = waterReservoirSurfaceSize * ( potHeight - distance );
    int waterLevel = (int) (waterContent / waterReservoirSize * 100);
    waterLevel = waterLevel < 0 ? 0: waterLevel;

    return waterLevel;
}
void activateWaterPump(unsigned long duration )
{
    Serial << F("[debug] Activating the water pump for: ") << duration << F("seconds") << endl;
    activateWaterPump();
    delay(duration);
    deactivateWaterPump();
}
void activateWaterPump()
{
    digitalWrite(waterPumpPin, HIGH );
}

void deactivateWaterPump()
{
    digitalWrite(waterPumpPin, LOW );
}
/**************************************************************************************************MQTT
 * MQTT
 */
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
        delay(5000);  // wait 5 seconds
        maxRetries--;

        Serial << F("[error] - Connecting to the MQTT broker failed it seems the broker is unavailable.") << endl;
        Serial << F("[info] - Halting the execution of the program.") << endl;
        while (1) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }

    Serial << "[info] - Successfully connected to the MQTT broker." << endl;
}

void verifyFingerprint()
{
    const char *host = mqttBrokerHost;
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
    buildPotStatisticMessage( (long)getMoistureLevel(), (int)calcWaterLevel());

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
    snprintf(jsonMessageSendBuffer, jsonBufferSize, potStatisticJsonFormat, potMacAddress, potStatisticCounter++, groundMoistureLevel, waterReservoirLevel);
}

void buildPotStatisticMessage(WarningType warningType)
{
    snprintf(jsonMessageSendBuffer, jsonBufferSize, potWarningJsonFormat, potMacAddress, potWarningCounter++, (char *) warningType);
}