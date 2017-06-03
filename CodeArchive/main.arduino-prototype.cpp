/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Created: 17-05-2017 05:02
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Arduino.h" // This library includes all functions etc to work with the Wemos D1 A2 board.
#include <ESP8266WiFi.h> // This library includes all functions to work with the ESP8222 Wireless communication chip.
#include <PubSubClient.h> // This library includes all functions to communicate messages over the MQTT protocol.
#include <ArduinoJson.h> // This library includes all the functions to parse and construct json messages.
#include <NewPing.h> // This library includes all the functions to work with the HC-SR04 ultra sonar sensor.

#define ledYellow D3 // Output pin connected to the yellow led.
#define ledRed D2 // Output pin connected to the red led.
#define ledPublish D5 // Output pin connected to the red led's that will be switched every MQTT publication.
#define triggerPin D6 // Trigger output pin for the ultra sonar senor.
#define echoPin D7 // Echo input pin the pin that listens for an echo signal.
#define maxSonarDistance 100 // The maximum amount of centimeters.
#define moistureSensorPin A0 // The analog input pin for the soil moisture sensor.

const char* ssid = "iot-test"; // An string containing the SSID of the network the pot will connect to.
const char* password = "test12345"; // The password to authenticate to the wifi network.
const char* mqttUsername = "inf1i-plantpot"; //
const char* mqttPassword = "password"; //
const char* mqtt_server = "broker.mqttdashboard.com"; // The address of the MQTT broker for publishing and receiving messages.

int8 lastState = 0;

WiFiClientSecure espClient; // This object will be used to talk over Wifi.
PubSubClient client(espClient); // This object will be used for MQTT communication.
StaticJsonBuffer<200> jsonBuffer; // This object will be used for JSON parsing.
NewPing sonar( triggerPin, echoPin, maxSonarDistance); // This object will be used to measure the distance to the water in the water reservoir.

int yellowLedStatus = 0; // The current status of the yellow led.
int redLedStatus = 0; // The current status of the red led.

long lastMessageMillis = 0; // The time of the last MQTT message publication.
char potMessage[300]; // An string containing the publication message.
int messagePublicationId = 0; // An counter for counting the amount of messages send.

/**
 * Initiate an wifi connection for communicating statistics and warnings to our MQTT broker.
 */
void setupWifi()
{
    delay( 10 ); // Fix for more sable wifi.
    Serial.printf("\nTrying to connect to SSID: %s\n", ssid); // Print the ssid from the network we are connecting to.
    WiFi.begin(ssid, password); // Start wifi communication.

    Serial.printf( "BSSID is: %s\n", WiFi.BSSIDstr().c_str() ); // Print the BSSID to serial for debugging.
    Serial.printf( "PlantPot mac address is: %s\nConnecting.", WiFi.macAddress().c_str() ); // Print the MAC address of the pot to serial for debugging.

    while (WiFi.status() != WL_CONNECTED) // Keep trying to connect to wifi.
    {
        delay(1000); // Wait 1 second before
        Serial.print('.');
    }

    randomSeed( micros() );

    Serial.printf("\nWiFi connected\nIP address:%s\n", WiFi.localIP().toString().c_str() ); // Print the ip address of the pot to serial for debugging.
}

/**
 * This function will get called when an message is send over MQTT to the plant pot.
 * @param topic The MQTT topic the pot is listening to.
 * @param payload The data in the MQTT body
 * @param length The length of the message.
 */
void callback( char* topic, byte* payload, unsigned int length )
{
    Serial.printf("Message arrived [%s]\n", topic); // Print the topic of the message received on serial for debugging.
    char inData[80]; // Create an new string to convert to json.

    for (int i = 0; i < length; i++) // For the length of the payload.
    {
        Serial.print( (char)payload[i] ); // Print each character stored in the payload for debugging.
        inData[i] = (char)payload[i]; // Convert bytes to chars.
    }

    JsonObject& root = jsonBuffer.parseObject(inData); // Create an JsonObject from the payload string.
    String color = root["color"]; // The color of the led to switch either red or yellow.
    int action = root["action"]; // Turn the led on or off? 0 or 1

    if( color == "red" )
    {
        redLedStatus = action; // Save the status to publish back over MQTT.
        digitalWrite( ledRed, (uint8_t) (action == 1 ? HIGH : LOW) ); // Switch the led state based on the message received.
        Serial.printf("\nLed color: red, action: %s\n", action ? "Turn on":"Turn Off"); // Print the action on serial for debugging.
    }
    else if( color == "yellow" )
    {
        yellowLedStatus = action; // Save the status to publish back over MQTT.
        digitalWrite(ledYellow, (uint8_t) (action == 1 ? HIGH : LOW)); // Switch the yellow state based on the message received.
        Serial.printf("\nLed color: YELLOW, action: %s ", action ? "Turn on":"Turn Off" ); // Print the action on serial for debugging.
    }
    else
    {
        Serial.println("\nError no action found.\n"); // The command send over MQTT is not recognized.
    }
}

/**
 * This function tries to connect to the MQTT broker.
 */
void connectMqtt()
{
    while (!client.connected()) // Loop until we're connected to the MQTT broker.
    {
        Serial.println("Attempting MQTT connection...");

        // Create a random client ID
        String clientId = "clientId-";
        clientId += String(random(0xffff), HEX);

        // Attempt to connect
        if ( client.connect( clientId.c_str() ))
        {
            Serial.println("connected"); // Print notification that we are connected to the MQTT broker.
            client.publish("stenden-waterup", "Your plant pot is connected and well.");// Announce that the plant pot is connected to the internet.
            client.subscribe("stenden-waterup-response");// Resubscribe to the stenden response  response topic
        }
        else
        {
            Serial.printf( "failed, rc= %s\ntry again in 5 seconds", client.state() ); // Print the error message
            delay(5000); // Wait 5 seconds before retrying
        }
    }
}

/**
 * This method will initiate the I/O pins and setup serial and wifi communication.
 */
void setup()
{
    pinMode( ledRed, OUTPUT);
    pinMode( ledYellow, OUTPUT);
    pinMode( ledPublish, OUTPUT);
    pinMode( triggerPin, OUTPUT);
    pinMode( echoPin, INPUT);

    digitalWrite( ledRed, LOW );
    digitalWrite( ledYellow, LOW );
    digitalWrite( ledPublish, LOW );

    Serial.begin( 115200 ); // Initialize the serial port so we can send debug data over it.
    setupWifi(); // Initialize the wifi module for network communication.
    client.setServer( mqtt_server, 1883 ); // Set the MQTT broker.
    client.setCallback( callback ); // Create an callback to listen for responses from the broker.
}

/**
 * This is the main program loop that controlles the plant pot and sends data over the internet.
 */
void loop()
{
    if (!client.connected()) // Setup the MQTT server.
    {
        connectMqtt();
    }

    client.loop();

    long now = millis(); // The time to calculate the MQTT publish interval
    if (now - lastMessageMillis > 10000) // Every 10 seconds spam the broker (y).
    {
        lastState = !lastState; // Switch the last state.
        digitalWrite(ledPublish, (uint8_t) (lastState ? HIGH : LOW)); // Switch the state of the publish led's.

        lastMessageMillis = now; // Save the last execution of this code.
        ++messagePublicationId; // Add one to the message publication count.

        snprintf (
                potMessage, // The string buffer to save the json to.
                300, // The buffer size of the json string.
                "{\"device\":\"plantpot\",\"messageId\": %ld,\"status\":{\"red\":%d,\"yellow\":%d,\"sonar\":%ld,\"moister\":\"%d\"}}", // The json string.
                (long)messagePublicationId, // The id of the message i.e. the publication counter since the last reboot.
                redLedStatus, // The current state of the red led light.
                yellowLedStatus, // The current state of the yellow led light.
                sonar.ping_cm(200), // The distance to the water in the water reservoir.
                analogRead(moistureSensorPin) // The value of the ground moister sensor.
        ); // Create an json string to publish to the MQTT broker.

        Serial.printf("Send message to the broker: %s\n", potMessage ); // Print an debug message with the publication to the serial port.
        client.publish("stenden-waterup", potMessage); // Publish the json message to the MQTT broker.
    }
}