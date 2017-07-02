/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Created: 17-06-2017 20:16
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Communication.h"

/**
 * The json string C-style formatted that will be filled with data and send to the mqtt broker.
 */
const char *potStatisticJsonFormat = "{\"mac\":\"%s\",\"type\":\"potstats-mesg\",\"counter\":%d,\"moisture\":%lu,\"waterLevel\":%d}";

/**
 * The json string C-style formatted that will be filled with data and send to the mqtt broker.
 */
const char *potWarningJsonFormat = "{\"mac\":\"%s\",\"type\":\"warning-mesg\",\"counter\":%d,\"warning\":\"%lu\"}";

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
Adafruit_MQTT_Client mqtt( &client, MQTT_BROKER_HOST, MQTT_BROKER_PORT, MQTT_BROKER_USERNAME, MQTT_BROKER_PASSWORD );

/**
 * Create the required publish clients that will be used to send messages to the mqtt broker
 * that will send it to the end users.
 */
Adafruit_MQTT_Publish statisticPublisher = Adafruit_MQTT_Publish( &mqtt, MQTT_BROKER_USERNAME TOPIC_PUBLISH_STATISTIC );
Adafruit_MQTT_Publish warningPublisher = Adafruit_MQTT_Publish( &mqtt, MQTT_BROKER_USERNAME TOPIC_PUBLISH_WARNING );

/**
 * Create the required subscribe clients that listen for incoming configuration messages send by the
 * mqtt broker.
 */
Adafruit_MQTT_Subscribe ledConfigListener = Adafruit_MQTT_Subscribe( &mqtt,  MQTT_BROKER_USERNAME TOPIC_SUBSCRIBE_LED_CONFIG );
Adafruit_MQTT_Subscribe mqttConfigListener = Adafruit_MQTT_Subscribe( &mqtt, MQTT_BROKER_USERNAME TOPIC_SUBSCRIBE_MQTT_CONFIG );
Adafruit_MQTT_Subscribe plantCareConfigListener = Adafruit_MQTT_Subscribe( &mqtt, MQTT_BROKER_USERNAME TOPIC_SUBSCRIBE_PLANT_CARE_CONFIG );

Configuration *Communication::potConfig = nullptr; // Initiate the static config variable with null.

/**
 * The constructor will initiate the communication library with some default
 * values and will save an reference to the configuration library.
 * @param potConfiguration  An pointer to the configuration library.
 */
Communication::Communication( Configuration *potConfiguration )
{
    potConfiguration->setup();
    delay( 1000 );
    Communication::potConfig = potConfiguration;
}

/**
 * This function initiates the communication settings. It will try to connect to the last
 * configured wifi network it it fails it will create an access point hosts an configuration
 * website where an user can connect to and set the wifi configuration.
 */
void Communication::setup()
{
    delay( 10 );
    Serial << endl;
    POT_DEBUG_PRINTLN( F( "[debug] - Setting up the communication library" ))

#ifdef POT_DEBUG
    WiFi.printDiag( Serial );
#endif

    WiFiManager wifiManager;
    wifiManager.autoConnect();

    while ( WiFi.waitForConnectResult() != WL_CONNECTED )
    {
        delay( 500 );
        Serial << F( "." );
    }

    POT_DEBUG_PRINTLN(
            F( "[info] - Successfully connected to the wifi network.\n" ) NEW_LINE
            F( "[debug] - IP address assigned from the router: " ) APPEND WiFi.localIP() NEW_LINE
            F( "[info] - Successfully connected to the wifi network.\n" ) NEW_LINE
            F( "[debug] - Plant pot mac address: " ) APPEND WiFi.macAddress())

    verifyFingerprint(); // Check SHA1 fingerprint of the MQTT broker.
    potMacAddress = WiFi.macAddress();
    this->listenForConfiguration();
}

/**
 * This function checks if there already is an mqtt connection if not it will attempt to open one. If
 * it fails to manny times it will kill its self so the user has to reset the plant pot.
 */
void Communication::connect()
{
    int8_t ret;
    uint8_t maxRetries = 50;

    if ( mqtt.connected())
    {
        return;
    }

    Serial << F( "[info] - Attempting to connect to the MQTT broker." ) << endl;

    while (( ret = mqtt.connect()) != 0 )
    { // connect will return 0 for connected
        POT_ERROR_PRINTLN( F( "[error] - Connecting to the MQTT broker failed because: " ) APPEND mqtt.connectErrorString( ret )) // Print an detailed error message.
        POT_DEBUG_PRINTLN( F( "[info] - Retrying to connect to the MQTT broker in 5 seconds..." ))

        mqtt.disconnect(); // Send disconnect package.
        delay( 2000 );  // wait 5 seconds
        maxRetries--;

        if ( maxRetries == 0 )
        {
            POT_ERROR_PRINTLN( F( "[error] - Connecting to the MQTT broker failed it seems the broker is unavailable." ))
            POT_DEBUG_PRINTLN( F( "[info] - Halting the execution of the program." ))
            while ( 1 ) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
            {
            }
        }
    }

    POT_DEBUG_PRINTLN( F("[info] - Successfully connected to the MQTT broker." ))
}

/**
 * This function will return the pointer to the configuration object that
 * contains communication and plant care settings.
 *
 * @return *Configuration
 */
Configuration *Communication::getConfiguration()
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
void Communication::publishStatistic( int groundMoistureLevel, int waterReservoirLevel )
{
    snprintf( jsonMessageSendBuffer, JSON_BUFFER_SIZE, potStatisticJsonFormat, WiFi.macAddress().c_str(), potStatisticCounter++, groundMoistureLevel, waterReservoirLevel );
    if ( !statisticPublisher.publish( jsonMessageSendBuffer )) // Did we publish the message to the broker?
    {
        POT_ERROR_PRINTLN( F( "[error] - Unable to send message: " ) APPEND jsonMessageSendBuffer )
    }
    else
    {
        POT_DEBUG_PRINTLN(
                F( "[debug] - Message with id: " ) APPEND potStatisticCounter APPEND F( " content: " ) APPEND jsonMessageSendBuffer NEW_LINE
                F( "[info] - Successfully published message to the MQTT broker." ) NEW_LINE
                F( "[debug] - Publish topic: " ) APPEND MQTT_BROKER_USERNAME APPEND TOPIC_PUBLISH_STATISTIC )
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
void Communication::publishWarning( uint8_t warningType )
{
    snprintf( jsonMessageSendBuffer, JSON_BUFFER_SIZE, potWarningJsonFormat, WiFi.macAddress().c_str(), potWarningCounter++, warningType );
    if ( !statisticPublisher.publish( jsonMessageSendBuffer )) // Did we publish the message to the broker?
    {
        POT_ERROR_PRINTLN( F( "[error] - Unable to send message: " ) APPEND jsonMessageSendBuffer )
    }
    else
    {
        POT_DEBUG_PRINTLN(
                F( "[debug] - Message with id: " ) APPEND potStatisticCounter APPEND F( " content: " ) APPEND jsonMessageSendBuffer NEW_LINE
                F( "[info] - Successfully published message to the MQTT broker." ))
    }
}

/**
 * This function will start listening for configuration send by the mqtt broker. It will register
 * the callback functions to the mqtt listeners so when an message is received it knows what function
 * to execute.
 */
void Communication::listenForConfiguration()
{
    POT_DEBUG_PRINTLN(
            F("[debug] - Start listening to configuration messages") NEW_LINE
            F("[debug] - Listening on:") APPEND MQTT_BROKER_USERNAME APPEND TOPIC_SUBSCRIBE_PLANT_CARE_CONFIG)

    ledConfigListener.setCallback( &Communication::listenForLedConfiguration );
    mqttConfigListener.setCallback( &Communication::listenForMqttConfiguration );
    plantCareConfigListener.setCallback( &Communication::listenForPlantCareConfiguration );

    mqtt.subscribe( &ledConfigListener );
    mqtt.subscribe( &mqttConfigListener );
    mqtt.subscribe( &plantCareConfigListener );
}

/**
 * This function will attempt to verify the TLS/SSL certificate send from the MQTT broker by its SHA1 fingerprint.
 * We use the SHA1 fingerprints instead of the complete certificates because of the memory limitations
 * of the arduino/huzzah. It will get the SHA1 send by the mqtt broker and compare it with the fingerprint hard
 * coded in the code stored on the plant pot.
 */
void Communication::verifyFingerprint()
{
    POT_DEBUG_PRINTLN( F( "[info] - Attempting to open an secure connection to the MQTT broker at:" ) APPEND MQTT_BROKER_HOST)

    if ( !client.connect( MQTT_BROKER_HOST, MQTT_BROKER_PORT ))
    {
        POT_ERROR_PRINTLN( F( "[error] - Connecting to the MQTT broker failed because we can't reach it." ))
        POT_DEBUG_PRINTLN( F( "[info] - Halting the execution of the program." ))
        while ( 1 ) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }

    if ( client.verify( MQTT_BROKER_FINGERPRINT, MQTT_BROKER_HOST ))
    {
        POT_DEBUG_PRINTLN( "[info] - Successfully verified the integrity of the TLS/SSL certificate send by the broker." )
    }
    else
    {
        POT_ERROR_PRINTLN( F("[error] - Connecting to the MQTT broker failed because the TLS/SSL certificate could not be verified." ))
        POT_DEBUG_PRINTLN(
                F( "[debug] - TLS/SSL SHA1 certificate fingerprint allowed: " ) APPEND MQTT_BROKER_FINGERPRINT NEW_LINE
                F( "[info] - Halting the execution of the program." ))
        while ( 1 ) // You shall not pass! Seriously this effectively kills the pot and you have to reset it or wait to the death of the universe.
        {
        }
    }
}

void Communication::listen()
{
    mqtt.processPackets(10);
}

void Communication::parseJsonData( char *messageData, uint16_t messageLength, uint8_t receivedOnListener )
{
    /*StaticJsonBuffer<JSON_BUFFER_SIZE> staticJsonReceiveBuffer;
    JsonObject &jsonRoot = staticJsonReceiveBuffer.parseObject( messageData );
    Serial.println(messageData);
    POT_ERROR_PRINTLN( F( "[debug] - Received data from the mqtt broker:" ) APPEND messageData )
    POT_ERROR_PRINTLN( F( "[debug] - Received data length" ) APPEND dataLength )
    if ( jsonRoot.success())
    {
        POT_ERROR_PRINTLN( F( "[error] - Error receiving json data the json data send by the broker is invalid" ))
        return;
    }

    if ( jsonRoot[ "mac" ] == false )
    {
        POT_ERROR_PRINTLN( F("[error] - Error receiving json data the mac and type nodes are missing." ))
        return;
    }

    String messageMacAddress = jsonRoot[ "mac" ];
    String messageName = jsonRoot[ "type" ];

    if ( not messageMacAddress.equals( WiFi.macAddress()))
    {
        POT_DEBUG_PRINTLN( F( "[debug] - The message received is not for us." ))
        return;
    }

    switch ( receivedOnListener )
    {
        case LED_LISTENER:
            POT_DEBUG_PRINTLN( F( "[debug] - Parsing json led configuration message." ))

            Communication::potConfig->setLedSettings(
                    ( uint8_t ) jsonRoot[ "red" ], // The new red led luminosity
                    ( uint8_t ) jsonRoot[ "green" ], // The new green led luminosity
                    ( uint8_t ) jsonRoot[ "blue" ] // The new blue led luminosity
            );
            break;

        case MQTT_LISTENER:
            POT_ERROR_PRINTLN( F( "[debug] - Parsing json mqtt configuration message." ))

            Communication::potConfig->setMQTTSettings(
                    ( uint32_t ) jsonRoot[ "stat-interval" ], // The new MQTT statistic publish interval
                    ( uint32_t ) jsonRoot[ "resend-interval" ], // The new MQTT resend warning interval
                    ( uint32_t ) jsonRoot[ "ping-interval" ], // The new MQTT ping interval
                    ( uint8_t ) jsonRoot[ "publish-threshold" ] // The new
            );
            break;

        case PLANT_CARE_LISTENER:
            POT_ERROR_PRINTLN( F( "[debug] - Parsing json plant care configuration message." ))

            Communication::potConfig->setPlantCareSettings(
                    ( uint8_t ) jsonRoot[ "moisture-need" ],
                    ( uint32_t ) jsonRoot[ "interval" ],
                    ( uint32_t ) jsonRoot[ "sleep-after-water" ],
                    ( uint8_t ) jsonRoot[ "contains-plant" ]
            );
            break;

        default:
            POT_ERROR_PRINTLN( F("[error] - Unknown configuration type." ))
            break;
    }*/
}

/**
 * This function callback will be subscribed to the plant care configuration topic.
 * When new plant care configuration gets published on this topic it will update the
 * pot's configuration.
 *
 * @param data      An json string containing plant care configuration.
 * @param messageLength    The length of the json string.
 */
void Communication::listenForLedConfiguration( char *messageData, uint16_t messageLength )
{
    POT_ERROR_PRINTLN( F( "[debug] - Received data from the mqtt broker:" ) APPEND messageData )
    POT_ERROR_PRINTLN( F( "[debug] - Received data length" ) APPEND messageLength )

    StaticJsonBuffer<JSON_BUFFER_SIZE> staticJsonReceiveBuffer;
    JsonObject &jsonRoot = staticJsonReceiveBuffer.parseObject( messageData );

    if ( jsonRoot.success())
    {
        POT_ERROR_PRINTLN( F( "[error] - Error receiving json data the json data send by the broker is invalid" ))
        return;
    }

    if ( jsonRoot[ "mac" ] == false )
    {
        POT_ERROR_PRINTLN( F("[error] - Error receiving json data the mac and type nodes are missing." ))
        return;
    }

    String messageMacAddress = jsonRoot[ "mac" ];

    if ( not messageMacAddress.equals( WiFi.macAddress()))
    {
        POT_DEBUG_PRINTLN( F( "[debug] - The message received is not for us." ))
        return;
    }

    Communication::potConfig->setPlantCareSettings(
            ( uint8_t ) jsonRoot[ "moisture-need" ],
            ( uint32_t ) jsonRoot[ "interval" ],
            ( uint32_t ) jsonRoot[ "sleep-after-water" ],
            ( uint8_t ) jsonRoot[ "contains-plant" ]
    );
}

/**
 * This function callback will be subscribed to the mqtt configuration topic.
 * When new mqtt configuration gets published on this topic it will update the
 * pot's configuration.
 *
 * @param data      An json string containing mqtt configuration.
 * @param length    The length of the json string.
 */
void Communication::listenForMqttConfiguration( char *messageData, uint16_t messageLength )
{
    POT_ERROR_PRINTLN( F( "[debug] - Received data from the mqtt broker:" ) APPEND messageData )
    POT_ERROR_PRINTLN( F( "[debug] - Received data length" ) APPEND messageLength )

    StaticJsonBuffer<JSON_BUFFER_SIZE> staticJsonReceiveBuffer;
    JsonObject &jsonRoot = staticJsonReceiveBuffer.parseObject( messageData );

    if ( jsonRoot.success())
    {
        POT_ERROR_PRINTLN( F( "[error] - Error receiving json data the json data send by the broker is invalid" ))
        return;
    }

    if ( jsonRoot[ "mac" ] == false )
    {
        POT_ERROR_PRINTLN( F("[error] - Error receiving json data the mac and type nodes are missing." ))
        return;
    }

    String messageMacAddress = jsonRoot[ "mac" ];

    if ( not messageMacAddress.equals( WiFi.macAddress()))
    {
        POT_DEBUG_PRINTLN( F( "[debug] - The message received is not for us." ))
        return;
    }

    Communication::potConfig->setPlantCareSettings(
            ( uint8_t ) jsonRoot[ "moisture-need" ],
            ( uint32_t ) jsonRoot[ "interval" ],
            ( uint32_t ) jsonRoot[ "sleep-after-water" ],
            ( uint8_t ) jsonRoot[ "contains-plant" ]
    );
}

/**
  * This function callback will be subscribed to the led configuration topic.
  * When new led configuration gets published on this topic it will update the
  * pot configuration.
  *
  * @param data      An json string containing led configuration.
  * @param length    The length of the json string.
  */
void Communication::listenForPlantCareConfiguration( char *messageData, uint16_t messageLength )
{
    POT_ERROR_PRINTLN( F( "[debug] - Received data from the mqtt broker:" ) APPEND messageData )
    POT_ERROR_PRINTLN( F( "[debug] - Received data length" ) APPEND messageLength )

    StaticJsonBuffer<JSON_BUFFER_SIZE> staticJsonReceiveBuffer;
    JsonObject &jsonRoot = staticJsonReceiveBuffer.parseObject( const_cast<char*>(messageData) );

    if ( jsonRoot.success())
    {
        POT_ERROR_PRINTLN( F( "[error] - Error receiving json data the json data send by the broker is invalid" ))
        return;
    }

    if ( jsonRoot[ "mac" ] == false )
    {
        POT_ERROR_PRINTLN( F("[error] - Error receiving json data the mac and type nodes are missing." ))
        return;
    }

    String messageMacAddress = jsonRoot[ "mac" ];

    if ( not messageMacAddress.equals( WiFi.macAddress()))
    {
        POT_DEBUG_PRINTLN( F( "[debug] - The message received is not for us." ))
        return;
    }

    Communication::potConfig->setPlantCareSettings(
            ( uint8_t ) jsonRoot[ "moisture-need" ],
            ( uint32_t ) jsonRoot[ "interval" ],
            ( uint32_t ) jsonRoot[ "sleep-after-water" ],
            ( uint8_t ) jsonRoot[ "contains-plant" ]
    );
}


