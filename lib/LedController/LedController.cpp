
/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 12-06-2017 11:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "LedController.h"

/**
 * Parameter 1 = number of pixels in strip
 * Parameter 2 = pin number (most are valid)
 * Parameter 3 = pixel type flags, add together as needed:
 * NEO_RGB     Pixels are wired for RGB bitstream
 * NEO_KHZ800  800 KHz bitstream (e.g. FLORA pixels)
 */
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_RGB + NEO_KHZ800);

bool oldState = HIGH;
int showType = 0;

unsigned long interval=50;  // the time we need to wait
unsigned long previousMillis=0;
uint32_t currentColor;// current Color in case we need it
uint16_t currentPixel = 0;// what pixel are we operating on


long previous = 0;
long current = 0;
uint8_t previousWaterLevelPosition = 0;


void LedController::setup()
{
    pinMode( PIXEL_PIN, OUTPUT );
    Serial << F("[info] - Starting led's on pin") << PIXEL_PIN << endl;

    currentColor = strip.Color(255,0,0);
    currentPixel = 0;
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    previous = millis();
}
void LedController::colorWipe(uint32_t c, uint8_t wait)
{
    for(uint16_t i=0; i<strip.numPixels(); i++)
    {
        strip.setPixelColor(i, c);
    }
}

void LedController::setColor(uint8_t r, uint8_t g, uint8_t b)
{
    colorWipe(strip.Color(g, r, b), 50);
    //delay(100);
    strip.show();
}


/**
 * Variables for led show
 */
uint8_t pos = 0;
uint8_t r = 200;
uint8_t  g = 0;
uint8_t  b = 0;
uint8_t loopNr = 0;

/**
 *
 *  Simple led show
 */
void LedController::ledShow(){

    strip.clear();
    uint32_t color = strip.Color(g,r,b);

    if((loopNr) % 60 == 0){
        r=200; g=0; b=0;
    }
    if((loopNr+20) % 60 == 0){
        r=0; g=200; b=0;
    }
    if((loopNr+40) % 60 == 0){
        r=0; g=0; b=200;
    }
    for(uint16_t i = pos; i < strip.numPixels(); i += 2){
        strip.setPixelColor(i, color);
    }
    strip.show();

    pos = pos == 0 ? 1 : 0;
    loopNr++;
    if(loopNr == 60)
    {
        loopNr = 0;
    }
    //delay(200);

}


/**
 *  Set color based on water level
 */
void LedController::setColorBasedOnWaterLevel(int waterLevel){

    current = millis();
    if( current - previous > 1000){

        if( waterLevel < 35 )
        {
            if(previousWaterLevelPosition != 1){
                for(int i = 0; i < 100;i++){
                    this->setColor( (uint8_t) i*1.5 , 0, 0 );
                    delay(10);
                }
            }
            previousWaterLevelPosition = 1;
            this->setColor( 150, 0, 0 );
        }
        else if( waterLevel < 50 )
        {
            if(previousWaterLevelPosition != 2){
                for(int i = 0; i < 100;i++){
                    this->setColor( i*2, i*1, 0 );
                    delay(10);
                }
            }
            previousWaterLevelPosition = 2;
            // Between red and yellow
            this->setColor( 200, 100, 0 );
        }
        else
        {
            if(previousWaterLevelPosition != 3){
                for(int i = 0; i < 100;i++){
                    this->setColor( 0, i*2, i*2 );
                    delay(10);
                }
            }
            previousWaterLevelPosition = 3;
            this->setColor( 0, 200, 200 );
        }
        previous = current;
    }
}




