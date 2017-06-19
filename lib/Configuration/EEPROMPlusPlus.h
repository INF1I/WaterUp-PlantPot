/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 19-06-2017 18:33
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_PLANTPOT_EEPROMPLUSPLUS_H
#define WATERUP_PLANTPOT_EEPROMPLUSPLUS_H

#include <EEPROM.h>
#include <Arduino.h>  // for type definitions

template <class T> int EEPROMWrite(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;

    for (i = 0; i < sizeof(value); i++)
    {
        EEPROM.write(ee++, *p++);
    }
    return i;
}

template <class T> int EEPROMRead(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;

    for (i = 0; i < sizeof(value); i++)
    {
        *p++ = EEPROM.read(ee++);
    }
        return i;
}

#endif //WATERUP_PLANTPOT_EEPROMPLUSPLUS_H
