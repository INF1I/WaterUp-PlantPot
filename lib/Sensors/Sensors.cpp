#include <Sensors.h>

#define trigPin 13
#define echoPin 12

#define soilSensePin A0


void Sensors::setupUltraSonic(){

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}
void Sensors::setupMoistureDetector(){
//    pinMode(wetIndicatorPin, OUTPUT);
//    pinMode(dryIndicatorPin, OUTPUT);
}

void Sensors::setup(){
    setupUltraSonic();
    setupMoistureDetector();
}


long Sensors::getDistance(){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long Duration = pulseIn(echoPin, HIGH); //Listening and waiting for wave
    delay(10);
    return (Duration*0.034/2);//Converting the reported number to CM
}


int Sensors::getMoistureLevel(){
    int v = analogRead(soilSensePin);
    return v/10;
}
