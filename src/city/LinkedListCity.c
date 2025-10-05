#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


#include "LinkedListCity.h"
#include "City.h"
#include "../weather/Weather.h"
#include "../mcore/json/cJSON/cJSON.h"
#include "../mcore/utils/CaseFormSwe.h"
#include "../mcore/utils/strdup.h"
#include "../mcore/utils/CreateDirectory.h"
#include "../mcore/json/fileHelper/fileHelper.h"

/* moved from City.c/.h */

/* Print displayName of nodes in LinkedListCities. Displays city names in a 5 row grid-like fashion*/
int LLC_DisplayLinkedListCities(LinkedListCities* _LLC)
{
	City* current = _LLC->head;
	if(current == NULL)
	{
		return LLC_DISPLAY_NOCITIESFOUND;
	}
    int i = 1; /* use an integer to print a select number of cities in a grid like view */
	do
	{
		printf("%s\t", current->displayName);
        if(strlen(current->displayName)< 8) printf("\t"); /* extra tab when displayName is too short */
		current = current->next;
        if (i % 5 == 0) printf("\n"); /* new line after 5 cities */
        i++;
	} while (current != NULL);
    printf("\n");
    return LLC_DISPLAY_OK;
}

/* Delete all linked list nodes.  Run at end of program execution to free memory. Calls Weather_DestroyReport to free any stored reports */
void LLC_DestroyLinkedListCities(LinkedListCities* _LLC) {
    City* current = _LLC->head;
    int i = 0;
    while (current != NULL) {
        City* next = current->next;
        /* printf("Freeing city: %s\n", current->displayName); */
        /* Free any dynamically allocated fields */

        /* if there is a weather report, free it! */
        if(current->WeatherData != NULL)
            Weather_DestroyReport(current->WeatherData);
        
        free(current->displayName); /* free allocd city name*/

        /* Free the node itself */
        free(current);

        current = next;
        i++;
    }
    /* printf("Total freed cities: %i\n", i); */
    _LLC->head = NULL;
    _LLC->tail = NULL;
}