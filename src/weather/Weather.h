#ifndef WEATHER_H
#define WEATHER_H

#include "../city/City.h"

int Weather_GetReport(City* _City);

void Weather_PrintReport(City* _City);
void Weather_PrintALL(City* _City);

void Weather_DestroyReport(WeatherReport* report);

#endif