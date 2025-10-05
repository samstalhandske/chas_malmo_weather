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

int LLC_DisplayLinkedListCities(LinkedListCities* _LLC);

void LLC_DestroyLinkedListCities(LinkedListCities* _LLC);


#endif