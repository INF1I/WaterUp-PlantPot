/**
 * Author: Joris Rietveld <jorisrietveld@gmail.com>
 * Author: Alwin Kroezen <alwin.kroesen@student.stenden.com>
 * Created: 17-06-2017 21:46
 * Licence: GPLv3 - General Public Licence version 3
 */
#include "PlantCare.h"

PlantCare::PlantCare(Communication *communication)
{
    this->communication = communication;
}

void PlantCare::setup()
{

}

void PlantCare::takeCareOfPlant()
{

}

int PlantCare::checkWaterReservoir()
{

}

int PlantCare::checkMoistureLevel()
{

}

void PlantCare::giveWater()
{

}

void PlantCare::giveWater( unsigned long duration )
{

}

void PlantCare::publishPotStatistic()
{

}

void PlantCare::publishPotWarning( WarningType warningType )
{

}