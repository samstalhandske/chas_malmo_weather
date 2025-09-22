#include <stdio.h>
#include <assert.h>
#include "cJSON/cJSON.h"
#include "../../../include/data.h"

#include "../http/http.h"

#ifndef GET_CURRENT_WEATHER_H
#define GET_CURRENT_WEATHER_H

typedef struct Weather_Report Weather_Report;

typedef struct Weather_Report{
    char* cityname; 
    double latitude;
    double longitude;
    long long timestamp;
    double windspeed;
    int temperature;
    int weathercode;
    char* description;
};

Weather_Report* Get_Weather_Report(char* _CityName, double _Latitude, double _Longitude);

long long StringTimeToLongLong(char* _stringTime);


#endif