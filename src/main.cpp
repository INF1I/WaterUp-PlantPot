/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Created: 17-05-2017 05:02
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define ledYellow D3
#define ledRed D2
#define ledPublish D5

const char* ssid = "iot-test";
const char* password = "test12345";
const char* mqtt_server = "broker.mqttdashboard.com";
int8 lastState = 0;

WiFiClient espClient;
PubSubClient client(espClient);
StaticJsonBuffer<200> jsonBuffer;
int yellowLedStatus = 0;
int redLedStatus = 0;

long lastMsg = 0;
char potMessage[200];
int value = 0;

void setup_wifi()
{
    delay( 10 ); // Fix for more sable wifi.
    Serial.printf("\nTrying to connect to SSID: %s", ssid); // Print the ssid from the network we are connecting to.
    WiFi.begin(ssid, password); // Start wifi communication.
    Serial.println();

    Serial.print( "BSSID is:");
    Serial.print( WiFi.BSSIDstr() );
    Serial.println();

    Serial.print( "PlantPot mac address is: ");
    Serial.print( WiFi.macAddress() );
    Serial.println();

    Serial.println( "Connecting." );
    while (WiFi.status() != WL_CONNECTED) // Keep trying to connect to wifi.
    {
        delay(1000);
        Serial.print('.');
    }

    randomSeed( micros() );

    Serial.printf("\nWiFi connected\nIP address:" );
    Serial.print(WiFi.localIP());
    Serial.println();
}

void callback( char* topic, byte* payload, unsigned int length )
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    char inData[80]; // Create an new string to convert to json.

    Serial.println();

    for (int i = 0; i < length; i++)
    {
        Serial.print( (char)payload[i] );
        inData[i] = (char)payload[i];
    }

    JsonObject& root = jsonBuffer.parseObject(inData);
    String color = root["color"]; // the color of the led either RED, Yellow or Blue
    int action = root["action"]; // "on" integer

    if( color == "red" )
    {
        redLedStatus = action;
        digitalWrite( ledRed, action == 1 ? HIGH : LOW );
        Serial.println("action: RED");
    }
    else if( color == "yellow" )
    {
        yellowLedStatus = action;
        digitalWrite( ledYellow, action == 1 ? HIGH : LOW );
        Serial.println("action: YELLOW");
    }
    else
    {
        Serial.println("Error no action found.");
    }
    Serial.println();
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
    pinMode( ledRed, OUTPUT);
    pinMode( ledYellow, OUTPUT);
    pinMode( ledPublish, OUTPUT);
    digitalWrite( ledRed, LOW );
    digitalWrite( ledYellow, LOW );
    digitalWrite( ledPublish, LOW );

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

    if (now - lastMsg > 10000) // Every 10 seconds spam the broker (y).
    {
        lastState = !lastState;

        if( lastState )
        {
            digitalWrite(ledPublish, HIGH );
        }
        else
        {
            digitalWrite(ledPublish, LOW );
        }

        lastMsg = now;
        ++value;
        snprintf (
                potMessage,
                200,
                "{\"device\":\"plantpot\",\"time\": %ld,\"messageId\": %ld,\"status\":{\"red\":%d,\"yellow\":%d}}",
                now,
                (long)value,
                redLedStatus,
                yellowLedStatus
        );

        Serial.printf("Send message to the broker: %s", potMessage );

        client.publish("stenden-waterup", potMessage);
    }
}