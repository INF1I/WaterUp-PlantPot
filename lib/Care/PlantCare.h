/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 17-06-2017 21:46
 * Licence: GPLv3 - General Public Licence version 3
 */
#ifndef WATERUP_PLANTPOT_PLANTCARE_H
#define WATERUP_PLANTPOT_PLANTCARE_H

class PlantCare;

enum WarningType
{
    LOW_RESORVOIR = 1,
    EMPTY_RESORVOIR = 2,
    LOW_MOISTURE_LEVEL = 3,
    HIGH_MOISTURE_LEVEL = 4,
    UNKNOWN_ERROR = 5
};

class PlantCare
{
public:
    PlantCare( Communication * communication );
    void setup();
    void takeCareOfPlant();

private:
    Communication communication;
    int checkWaterReservoir();
    int checkMoistureLevel();
    void giveWater();
    void giveWater( unsigned long duration );
    void publishPotStatistic();
    void publishPotWarning( WarningType warningType );
};


#endif //WATERUP_PLANTPOT_PLANTCARE_H
