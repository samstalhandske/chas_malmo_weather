#ifndef WEATHER_H
#define WEATHER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "../mcore/json/cJSON/cJSON.h"
#include "../city/City.h"
#include "../mcore/http/http.h"

int Weather_GetReport(City* _City);

void Weather_PrintReport(City* _City);
void Weather_PrintALL(City* _City);

void Weather_DestroyReport(WeatherReport* report);

#endif