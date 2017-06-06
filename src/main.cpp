/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Arduino.h"
#include <MQTT.h>
#include <Sensors.h>


MQTT mqtt;
Sensors s;


/************************* Values for the plant pot *********************************/
#define potLength 30
#define potWidth 30
#define potHeight 40 // Height what the water can reach
#define innerPotLength 20
#define innerPotWidth 20



/**
 * Calculate water level in the pot
 * Returns water percentage
 *
 * @param distance
 * @return waterLevel
 *
 */
int calcWaterLevel(float distance){
    long surface = (potLength * potWidth) - (innerPotLength * innerPotWidth);
    float content = surface * potHeight;
    float waterContent = surface * ( potHeight - distance );
    int waterLevel = (int) (waterContent / content * 100);
    if(waterLevel < 0) waterLevel = 0;
    return waterLevel;
}




/**
 * This function is used to initiate the Arduino/Huzzah board. It gets executed whenever the board is
 * first powered up or after an rest.
 */
void setup()
{

    Serial.begin(115200); // Start serial communication for sending debug messages to the serial port.
    delay(10); // Fix to make connecting to the wifi network more stable.


    mqtt.setup();
    s.setup();

}
/**
 * This function will run as long as the board is powered on, it contains the main program code.
 */
void loop()
{
    long distance = s.getDistance();

    mqtt.mqttConnect();
    mqtt.buildPotStatisticMessage((uint8_t) s.getMoistureLevel(),(uint8_t) calcWaterLevel(distance));

    mqtt.publish();

    delay(30000);
}
