
/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 12-06-2017 11:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#include <LedController.h>

/**
 * Parameter 1 = number of pixels in strip
 * Parameter 2 = pin number (most are valid)
 * Parameter 3 = pixel type flags, add together as needed:
 * NEO_RGB     Pixels are wired for RGB bitstream
 * NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
 */
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

 bool oldState = HIGH;
 int showType = 0;


 void LedController::setup()
 {
     pinMode( PIXEL_PIN, OUTPUT );
     Serial << F("[info] - Starting led's on pin") << PIXEL_PIN << endl;

     strip.begin();
     strip.show(); // Initialize all pixels to 'off'
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
     colorWipe(strip.Color(r, g, b), 50);
     delay(100);
     strip.show();
 }