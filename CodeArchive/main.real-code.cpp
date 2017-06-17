/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#include <Arduino.h> // This is the basic Arduino/Huzzah library.
#include <MQTT.h> // This is our library that is used to connect to the internet.
#include <Sensors.h> // This is our library that is used to collect plant pot data.
#include <LedController.h> // This is our library that is used to control the led-strip
#include <WiFiManager.h> // This is an library that sets the wifi password.

MQTT mqtt; // Create an new MQTT object for communication with the broker.
Sensors sensors; // Create an new Sensors object for taking measurements about the pot state.
LedController ledStrip; // Create an new LedController object for handling led's on the led-strip

/**
 * This function is used to initiate the Arduino/Huzzah board. It gets executed whenever the board is
 * first powered up or after an rest.
 */
void setup()
{
    delay(10000);
    Serial.begin(115200); // Start serial communication for sending debug messages to the serial port.
    delay(10); // Fix to make connecting to the wifi network more stable.
    mqtt.setup( &sensors ); // Setup the mqtt library.
}

/**
 * This function will run as long as the board is powered on, it contains the main program code.
 */
void loop()
{
    mqtt.mqttConnect(); // Connect to the broker if the connection is lost.
    mqtt.runLoop(); // Run the main program.
}
