#ifndef __CITY_H__
#define __CITY_H__

#include <stdbool.h>

typedef struct City City;

typedef struct LinkedListCities LinkedListCities;

typedef struct WeatherReport WeatherReport;

struct City {
    City* prev;
    City* next;

    char* displayName;
    double latitude;
    double longitude;
    WeatherReport* WeatherData;
};

struct WeatherReport{
    char* cityname; /* uses same name as City, no need to free */
    double latitude;
    double longitude;
    long long timeLongNumber; /* time as number (huge number)*/
    char* timeISO8601; /* new: text string time from json data. ! allocd char str !*/
    double precipitation; /* new */
    char* precipitationUnits; /* new free after use */
    int temperature; /* °C ?*/
    char* temperatueUnits;  /* allocd char str*/
    int elevation; /* new: elevation data*/
    int weathercode; /* 0-100 */
    char* weathercodeVerbose; /* verbose interpretation of WMO Weather Code*/
    double windspeed;
    char* windspeedUnits; /* allocd char str*/
    double windspeedMS; /* converted to m/s */
    double windspeedKMPH; /* default windspeed */
    int windDirection; /* new  */
    char* windDirectionUnits; /* allocd char string ! */
    char* windDirectionVerbose;
    bool isDay; /* new isDay day = 1 or night = 0 */
};

/* enums for CITY functions @(SS) Vi behöver ha lite ENUM input på det här*/
typedef enum {
    CITY_PARSE_OK = 0,
    CITY_PARSE_FAIL = -1,
    CITY_ADDTOLINKEDLIST_OK = 0,
    CITY_ADDTOLINKEDLIST_FAIL = -1,
    CITY_INIT_OK = 0,
    CITY_INIT_FAIL = -1
} City_ReturnENUMS;

typedef enum {
    CITY_SAVETOJSON_OK = 0,
    CITY_SAVETOJSON_FAIL = -1
} City_SaveToJsonENUMS;


int City_InitializeCitySystem(LinkedListCities* _LLC);

int City_AddCityToLinkedList(LinkedListCities* _LLC, char* _DisplayName, double _Latitude, double _Longitude, City** _CityPtr);
int City_RemoveCityFromLinkedList(LinkedListCities* _LLC, City* _City);
City* City_FindCity(LinkedListCities* _LLC, const char* _Name);

int City_EditCity(LinkedListCities* _LLC, const char* _CityName, const char* _NewName, const char* _Latitude, const char* _Longitude);



int City_SaveToJsonFile(City* _City);

/*
void buildDatabase();
void listCities();
int citySelect();
City* fetchCity(int choice);
*/

#endif