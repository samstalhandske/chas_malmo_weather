#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "ui/ClientUI.h"
#include "city/City.h"
#include "weather/Weather.h"
#include "mcore/http/http.h"
#include "mcore/json/json.h"
#include "mcore/json/fileHelper/fileHelper.h"

int main() {
	bool programShouldExit;
	LinkedListCities LLC;
	
	if(City_InitializeCitySystem(&LLC) == CITY_INIT_OK){
		programShouldExit = false;
		printf("<===================# Welcome to the Weather App! #===================>\n\n");
	}else{
		printf("City_Initialize failed");
		programShouldExit = true;
	}
	
	while (programShouldExit == false) {
		if(LLC_DisplayLinkedListCities(&LLC) == LLC_DISPLAY_NOCITIESFOUND)
			printf("\n\nNo cached cities found!\n\n");

	    printf("\nOptions(O/0) for options, Exit(E/Q) to exit.\nSelect a city: ");
		char* userInput = ClientUI_GetUserInputChar();
		/* printf("Recorded input: %s\n", userInput); */

		if (strcmp(userInput, "Exit") == 0 || strcmp(userInput, "Quit") == 0 || strcmp(userInput, "Q") == 0 || strcmp(userInput, "E") == 0){
			programShouldExit = true;
		}
		else if (strcmp(userInput, "Options") == 0 || strcmp(userInput, "0") == 0 || strcmp(userInput, "O") == 0){
			ClientUI_Options(&LLC);
		}
		else{
			City* selectedCity = City_FindCity(&LLC, userInput);
			if(selectedCity == NULL){
				printf("\n\t\"%s\" >> No matching city name found!\n\n", userInput);
			}
			else
			{	
				Weather_GetReport(selectedCity);
				
				Weather_PrintReport(selectedCity);			
			}
		}
		free(userInput);
	}
	LLC_DestroyLinkedListCities(&LLC);
	/* printf("Exiting weather app...\n"); */
	return 0;
}