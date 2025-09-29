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

int main() {
	bool programShouldExit;
	LinkedListCities LLC;
	
	if(City_InitializeCitySystem(&LLC) == 0){
		programShouldExit = false;
		
	}else{
		printf("City_Initialize failed");
		programShouldExit = true;
	}
	
	printf("<==============# Welcome to the Weather App! #==============>\n");
	
	/*
	buildDatabase();
	
	cJSON* my_JSON_Object = Read_JSON_From_File("file.json");
	if(my_JSON_Object == NULL){
		my_JSON_Object = cJSON_CreateObject();
		}
		printf("%s\n", cJSON_Print(my_JSON_Object));
		Write_JSON_To_File("file.json", my_JSON_Object);	
		*/
	
	while (programShouldExit == false) {
		City_DisplayLinkedListCities(&LLC);

		char* userInput = UserSelectCityChar();
		printf("Recorded input: %s\n", userInput);

		if (strcmp(userInput, "Exit") == 0 || strcmp(userInput, "Quit") == 0 || strcmp(userInput, "Q") == 0){
			programShouldExit = true;
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