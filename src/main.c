#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "ui/UserInteractions.h"
#include "city/City.h"
#include "weather/Weather.h"
#include "mcore/http/http.h"
#include "mcore/json/json.h"
#include "mcore/json/fileHelper/fileHelper.h"

#include <locale.h>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS /* Fixes deprecated warnings on Windows for sprintf, etc */
#include <Windows.h>
#endif

int main() {


#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif

	bool programShouldExit;
	LinkedListCities LLC;
	
	if(City_InitializeCitySystem(&LLC) == 0){
		programShouldExit = false;
		
	}else{
		printf("City_Initialize failed");
		programShouldExit = true;
	}
	
	printf("<==============# Welcome to the Weather App! #==============>\n");
	
	while (programShouldExit == false) {
		City_DisplayLinkedListCities(&LLC);

	    printf("\nOptions for options, Exit (Quit or Q) to exit.\nSelect a city: ");
		char* userInput = UserSelectCityChar();
		printf("Recorded input: %s\n", userInput);

		if (strcmp(userInput, "Exit") == 0 || strcmp(userInput, "Quit") == 0 || strcmp(userInput, "Q") == 0){
			programShouldExit = true;
		}
		else if (strcmp(userInput, "Options") == 0 || strcmp(userInput, "0") == 0){
			UserSelectOptions(&LLC);
			continue;
		}
		else{
			City* selectedCity = City_FindCity(&LLC, userInput);
			if(selectedCity == NULL){
				printf("No matching city name found!\n");
				continue;
			}
			
			WeatherReport CurrentWeather = Weather_GetReport(selectedCity->displayName, selectedCity->latitude, selectedCity->longitude);
			Weather_Print(&CurrentWeather);			

		}
		free(userInput);
	}
	City_DestroyLinkedListCities(&LLC);
	printf("Exiting weather app...\n");
	return 0;
}