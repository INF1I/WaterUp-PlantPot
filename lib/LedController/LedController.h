/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 01-06-2017 13:00
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_LEDCONTROLLER_LEDCONTROLLER_H
#define WATERUP_LEDCONTROLLER_LEDCONTROLLER_H

#ifndef LEDCONTROLLER_DEBUG_MODE
#define LEFCONTROLLER_DEBUG_MODE 1
#endif

#include <Arduino.h> // Include this library so we can use the arduino system functions and variables.
#include <Adafruit_NeoPixel.h> // Include this library for handling leds.
#include <Streaming.h>

#define PIXEL_PIN 14    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 30  // Number of led's


class LedController;

/**
 * This class is used to control the led's.
 */
class LedController
{
private:
    /**
     * Fill the dots one after the other with a color
     * @param c  Adafruit_NeoPixel.color(R, G, B)
     * @param wait
     */
    void colorWipe(uint32_t c, uint8_t wait);
public:
    /**
     * Initiate led's.
     */
    void setup();

    /**
     * Set the color of the strip
     * @param r Red color (0-255)
     * @param g Green color (0-255)
     * @param b Blue color (0-255)
     */
    void setColor(uint8_t r, uint8_t g, uint8_t b);


};

#endif // WATERUP_LEDCONTROLLER_LEDCONTROLLER_H