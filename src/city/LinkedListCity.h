/* moved from City.c/.h */
#ifndef __LINKEDLISTCITY_H__
#define __LINKEDLISTCITY_H__

typedef struct City City;

typedef struct LinkedListCities LinkedListCities;

struct LinkedListCities {
    City* head;
    City* tail;
};

typedef enum {
    LLC_DISPLAY_OK = 0,
    LLC_DISPLAY_NOCITIESFOUND = 1,
    LLC_DISPLAY_FAIL = -1
}LLC_DisplayENUMS;

int City_InitializeCitySystem(LinkedListCities* _LLC);

int City_RemoveCityFromLinkedList(LinkedListCities* _LLC, City* _City);

int LLC_DisplayLinkedListCities(LinkedListCities* _LLC);

void LLC_DestroyLinkedListCities(LinkedListCities* _LLC);

int City_AddCityToLinkedList(LinkedListCities* _LLC, char* _DisplayName, double _Latitude, double _Longitude, City** _CityPtr);

City* City_FindCity(LinkedListCities* _LLC, const char* _Name);

int City_EditCity(LinkedListCities* _LLC, const char* _CityName, const char* _NewName, const char* _Latitude, const char* _Longitude);


#endif