
/***************************************************
  Adafruit MQTT Library ESP8266 Adafruit IO SSL/TLS example
  Must use the latest version of ESP8266 Arduino from:

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Tony DiCola for Adafruit Industries.
  SSL/TLS additions by Todd Treece for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <MQTT.h>

/************************* WiFi Access Point *********************************/

//#define WLAN_SSID       "ZyXEL32C34A"
//#define WLAN_PASS       "FamilieKroesen"
#define WLAN_SSID       "ASUS-ALWIN"
#define WLAN_PASS       "test12345"
//
//#define WLAN_SSID       "iot-test"
//#define WLAN_PASS       "test12345"

/************************* Adafruit.io Setup *********************************/

#define mqttBrokerHost      "mqtt.inf1i.ga"
#define mqttBrokerPort  8883                   // 8883 for MQTTS
#define mqttBrokerUsername    "inf1i-plantpot"
#define mqttBrokerPassword         "password"


#define publishStatisticsTopic "/publish/statistics" // This MQTT topic is used to publish pot state statistics.
#define publishWarningTopic "/publish/warning" // This MQTT topic is used to publish pot warnings such as empty water reservoir warnings.
#define subscribeConfigTopic "/subscribe/config" // This MQTT topic is used receive new pot configuration.

#define jsonBufferSize 200 // This holds the default string buffer size of json messages.


/**
 * The SHA1 fingerprints taken from the backend server's SSL certificates.
 */
const char *apiInf1iGaFingerprint = "ED 1D 2C E2 41 5D 35 81 F6 15 DB A1 C8 0B 19 71 32 67 8B 46";
const char *mqttInf1iGaFingerprint = "A6 E4 A9 8C 92 B3 8D 81 73 CE 5B 33 33 F5 A3 7A 1B 87 E2 F3";

/**
 * Define JSON message templates that will be filled with data when send to an broker.
 */
const char *potStatisticJsonFormat = "{\"mac\":\"%s\",\"type\":\"potstats-mesg\",\"counter\":%d,\"moisture\":%d,\"waterLevel\":%d}";
const char *potWarningJsonFormat = "{\"mac\":\"%s\",\"type\":\"warning-mesg\",\"counter\":%d,\"warning\":\"%d\"}";

char jsonMessageSendBuffer[jsonBufferSize];
char jsonMessageReceiveBuffer[jsonBufferSize];

const char *potMacAddress = "";

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




void MQTT::setup() {


    // Connect to WiFi access point.
    Serial.println(); Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);

    delay(1000);

    WiFi.begin(WLAN_SSID, WLAN_PASS);
    delay(2000);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.println("WiFi connected");
    Serial.println("IP address: "); Serial.println(WiFi.localIP());

    // check the fingerprint of io.adafruit.com's SSL cert
    verifyFingerprint();

}


void MQTT::verifyFingerprint() {

    const char* host = mqttBrokerHost;


    Serial.print("Connecting to ");
    Serial.println(host);
    if (! wifiClient.connect(host, mqttBrokerPort)) {
        Serial.println("Connection failed. Halting execution.");
        while(1);
    }

    if (wifiClient.verify(mqttInf1iGaFingerprint, host)) {
        Serial.println("Connection secure.");
    } else {
        Serial.println("Connection insecure! Halting execution.");
        while(1);
    }

}




// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT::MQTT_connect() {
    int8_t ret;

    // Stop if already connected.
    if (mqttClient.connected()) {
        return;
    }

    Serial.print("Connecting to MQTT... ");

    uint8_t retries = 3;
    while ((ret = mqttClient.connect()) != 0) { // connect will return 0 for connected
        Serial.println(mqttClient.connectErrorString(ret));
        Serial.println("Retrying MQTT connection in 5 seconds...");
        mqttClient.disconnect();
        delay(5000);  // wait 5 seconds
        retries--;
        if (retries == 0) {
            // basically die and wait for WDT to reset me
            while (1);
        }
    }

    Serial.println("MQTT Connected!");
}



void MQTT::mqttConnect()
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
    /**
     * Everything went fine we are now connected to the MQTT broker.
     */
    Serial << "[info] - Successfully connected to the MQTT broker." << endl;
}








uint32_t x=0;
void MQTT::publish() {
    statisticPublisher.publish(jsonMessageSendBuffer);

    if (!statisticPublisher.publish(jsonMessageSendBuffer)) // Did we publish the message to the broker?
    {
//        Serial.println( "[error] - Unable to send message: " + String.valueOf( jsonMessageSendBuffer ) );
    }
    else
    {
//        Serial.println( "[debug] - Message with id: " + String.valueOf( potStatisticCounter ) );
//        Serial.println( "[info] - Successfully published message to the MQTT broker." );
    }
}


/**
 * Function for constructing an JSON pot statistic message.
 * @param groundMoistureLevel The measurement data from the soil humidity sensor.
 * @param waterReservoirLevel The measurement data from the ultra sonar sensor in the water reservoir.
 */
void MQTT::buildPotStatisticMessage(uint16_t groundMoistureLevel, uint8_t waterReservoirLevel)
{

    // Create the JSON message based on the specified format and write it to the send buffer.
    snprintf(jsonMessageSendBuffer, jsonBufferSize, potStatisticJsonFormat, potMacAddress, potStatisticCounter++, groundMoistureLevel, waterReservoirLevel);
}

/**
 * Function for constructing an JSON pot warning message.
 * @param warningType The type of warning that needs to be send.
 */
void MQTT::buildPotStatisticMessage( WarningType warningType )
{
    // Create the JSON message based on the specified format and write it to the send buffer.
    snprintf(jsonMessageSendBuffer, jsonBufferSize, potWarningJsonFormat, potMacAddress, potWarningCounter++, (char *)warningType);
}
