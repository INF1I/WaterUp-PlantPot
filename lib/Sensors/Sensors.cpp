/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#include <Sensors.h>

#define trigPin 13
#define echoPin 12
#define soilSensePin A0

void Sensors::setupUltraSonic()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void Sensors::setupMoistureDetector()
{
//    pinMode(wetIndicatorPin, OUTPUT);
//    pinMode(dryIndicatorPin, OUTPUT);
}

void Sensors::setup()
{
    setupUltraSonic();
    setupMoistureDetector();
}


long Sensors::getDistance()
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


int Sensors::getMoistureLevel()
{
    int v = analogRead(soilSensePin);
    return v / 10;
}

/**
 * Calculate water level in the pot
 *
 * @return waterLevel The percentage of water in the resorvoir.
 */
int Sensors::calcWaterLevel()
{
    float distance = getDistance();
    long surface = (potLength * potWidth) - (innerPotLength * innerPotWidth);
    float content = surface * potHeight;

    float waterContent = surface * ( potHeight - distance );
    int waterLevel = (int) (waterContent / content * 100);
    if(waterLevel < 0) waterLevel = 0;
    return waterLevel;
}