#ifndef __CITY_H__
#define __CITY_H__

typedef struct City City;
typedef struct LinkedListCities LinkedListCities;

struct City {
    City* prev;
    City* next;

    char* displayName;
    double latitude;
    double longitude;
};

struct LinkedListCities {
    City* head;
    City* tail;
};


typedef struct
{
    char* displayName;
    double latitude;
    double longitude;
} City_Info;

int City_InitializeCitySystem(LinkedListCities* _LLC);

int City_AddCityToLinkedList(LinkedListCities* _LLC, char* _DisplayName, double _Latitude, double _Longitude, City** _CityPtr);
int City_RemoveCityFromLinkedList(LinkedListCities* _LLC, City* _City);
void City_DisplayLinkedListCities(LinkedListCities* _LLC);
City* City_FindCity(LinkedListCities* _LLC, char* _Name);

int City_EditCity(LinkedListCities* _LLC, City* _City, City_Info* _CityInfo);

void City_DestroyLinkedListCities(LinkedListCities* _LLC);

/*
void buildDatabase();
void listCities();
int citySelect();
City* fetchCity(int choice);
*/

#endif