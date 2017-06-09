/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#include <Sensors.h>

/**
 * Initiate all sensors.
 */
void Sensors::setup()
{
    pinMode( trigPin, OUTPUT );
    pinMode( echoPin, OUTPUT );
    pinMode( soilSensePin, INPUT );
    pinMode( waterPumpPin, OUTPUT );
    pinMode( ledDataPin, OUTPUT );
    digitalWrite( waterPumpPin, LOW );
}

/**
 * Get the distance in centimeters to the water in the water rersorvoir.
 * @return long The distance to the water.
 */
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

/**
 * Get the soil resistance mesured by the soil moisture sensor.
 * @return int The resistance of the soil.
 */
int Sensors::getMoistureLevel()
{
    int v = analogRead(soilSensePin);
    return v / 10;
}

/**
 * Calculate the percentage of water left in the water reservoir.
 * @return waterLevel The percentage of water left in the reservoir.
 */
int Sensors::calcWaterLevel()
{
    float distance = getDistance();
    float waterContent = waterReservoirSurfaceSize * ( potHeight - distance );
    int waterLevel = (int) (waterContent / waterReservoirSize * 100);
    waterLevel = waterLevel < 0 ? 0: waterLevel;

    return waterLevel;
}

/**
* This function will activate the water pump for an specified duration.
*/
void Sensors::activateWaterPump(unsigned long duration )
{
    Serial << F("[debug] Activating the water pump for: ") << duration << F("seconds") << endl;
    activateWaterPump();
    delay(duration);
    deactivateWaterPump();
}

void Sensors::activateWaterPump()
{
    digitalWrite(waterPumpPin, HIGH );
}

void Sensors::deactivateWaterPump()
{
    digitalWrite(waterPumpPin, LOW );
}