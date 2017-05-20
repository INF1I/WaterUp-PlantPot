/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Created: 17-05-2017 05:02
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WaterUpTalk.h"

const char* ssid = "iot-test";
const char* password = "test12345";
const char* mqtt_server = "broker.mqttdashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char potMessage[50];
int value = 0;

void setup_wifi()
{
    delay(10); // Fix for more sable wifi.

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.printf("Connecting to %s", ssid); // Print the ssid from the network we are connecting to.

    WiFi.begin(ssid, password); // Start wifi communication.

    while (WiFi.status() != WL_CONNECTED) // Keep trying to connect to wifi.
    {
        delay(500);
        Serial.print(".");
    }

    randomSeed( micros() );

    Serial.printf("\nWiFi connected\nIP address:" );
    Serial.print(WiFi.localIP());
}

void callback( char* topic, byte* payload, unsigned int length )
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    for (int i = 0; i < length; i++)
    {
        Serial.print( (char)payload[i] );
    }

    Serial.println();

    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1')
    {
        digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is acive low on the ESP-01)
    }
    else
    {
        digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }

}

void reconnect()
{

    while (!client.connected()) // Loop until we're reconnected
    {
        Serial.print("Attempting MQTT connection...");

        // Create a random client ID
        String clientId = "clientId-";
        clientId += String(random(0xffff), HEX);

        // Attempt to connect
        if ( client.connect( clientId.c_str() ))
        {
            Serial.println("connected");
            client.publish("stenden-waterup", "Your plant pot is connected and well.");// Announce that the plant pot is connected to the internet.
            client.subscribe("stenden-waterup-response");// Resubscribe to the stenden response  response topic
        }
        else
        {
            Serial.printf( "failed, rc= %s", client.state() );
            Serial.println(" try again in 5 seconds");
            delay(5000); // Wait 5 seconds before retrying
        }
    }
}

void setup()
{
    pinMode( BUILTIN_LED, OUTPUT );     // Initialize the BUILTIN_LED pin as an output
    Serial.begin( 115200 ); // Initialize the serial port so we can send debug data over it.
    setup_wifi(); // Initialize the wifi module for network communication.
    client.setServer( mqtt_server, 1883 ); // Set the MQTT broker.
    client.setCallback( callback ); // Create an callback to listen for responses from the broker.
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    client.loop();

    long now = millis();
    if (now - lastMsg > 2000) // Every 2 seconds spam the broker (y).
    {
        lastMsg = now;
        ++value;
        snprintf ( potMessage, 75, "Your pot is on fire! It will burn your house down! #%ld", value);
        Serial.printf("Send message to the broker: %s", potMessage );

        client.publish("stenden-waterup", potMessage);
    }
}