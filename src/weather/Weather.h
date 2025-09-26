#ifndef GET_CURRENT_WEATHER_H
#define GET_CURRENT_WEATHER_H

#include <stdio.h>
#include <assert.h>
#include "../mcore/json/cJSON/cJSON.h"
#include "../city/City.h"
#include "../mcore/http/http.h"

typedef struct {
    char* cityname; 
    double latitude;
    double longitude;
    long long timestamp;
    double windspeed;
    int temperature;
    int weathercode;
    char* description;
} WeatherReport;

WeatherReport* Weather_GetReport(char* _CityName, double _Latitude, double _Longitude);

void Weather_DestroyReport(WeatherReport* report);

#endif