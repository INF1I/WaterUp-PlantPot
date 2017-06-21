/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 19-06-2017  03:52
 * Licence: GPLv3 - General Public Licence version 3
 */

#include <Arduino.h>
#include <Communication.h>
#include <Streaming.h>

long previousMillisStatistics = 0;
long statisticMillisInterval = 1000;

Communication communication;

void setup()
{
    Serial.begin(112500);
    communication.setup();
    previousMillisStatistics = millis();
}

void loop()
{
    communication.connect();

    unsigned long currentMillis = millis();

    if( currentMillis - previousMillisStatistics == statisticMillisInterval )
    {
        previousMillisStatistics = currentMillis;
        communication.publishStatistic(random(0,100),random( 0, 100 ));
    }
}