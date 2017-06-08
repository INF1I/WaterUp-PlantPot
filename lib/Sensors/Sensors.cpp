/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "Sensors.h"

/**
 * Initiate tha/bin/../lib/gcc/xtensa-lx106-elf/4.8.2/../../../../xtensa-lx106-elf/bin/ld: cannot find -lhandlers-sim
/home/ubuntu/.platformio/packages/toolchain-xtensa/bin/../lib/gcc/xtensa-lx106-elf/4.8.2/../../../../xtensa-lx106-elf/bin/ld: cannot find -lhal
collect2: error: ld returned 1 exit status
CMakeFiles/WaterUp-PlantPot.dir/build.make:120: recipe for target 'WaterUp-PlantPot' failed
make[3]: *** [WaterUp-PlantPot] Error 1e I/O pin's connected to the ultrasonic sensor.
 */
void Sensors::setupUltraSonic()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

/**
 * Initiate the I/O pin's connected to the soil moister sensor.
 */
void Sensors::setupMoistureDetector()
{
    pinMode( soilSensePin, INPUT );
}

/**
 * Initiate all sensors.
 */
void Sensors::setup()
{
    setupUltraSonic();
    setupMoistureDetector();
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