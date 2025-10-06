#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ui/ClientUI.h"
#include "../mcore/json/cJSON/cJSON.h"
#include "../mcore/utils/CaseFormSwe.h"
#include "../mcore/utils/strdup.h"
#include "../city/City.h"
#include "../city/LinkedListCity.h"
#include "../mcore/utils/dtos.h"
#include "../weather/Weather.h"
/*#include "../mcore/json/fileHelper/fileHelper.h"*/

/*#include "mcore/http/http.h"*/
/*#include "mcore/json/json.h"*/
/*#include "mcore/json/fileHelper/fileHelper.h"*/

/* Returns memory allocd char* string.  remember to free() after use! Formats user input to UPPERCASE first character, rest lowercase characters */
char* ClientUI_GetUserInputChar(){
/* Get user input and format with FormatUserInput */
    char buffer[100];
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    char* returnString = strdup(buffer);
    CaseFormatSwedish(returnString);

    return returnString;
}

void ClientUI_Run(){
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
}

/* Options UI. magic switch case for maximized user comfort. No warranty, no refunds and no responsibility. */
void ClientUI_Options(LinkedListCities* _LLC){

    int iOption;
    int c;
    printf("\n\t1. Add City\n\t2. Remove City\n\t3. Edit city\n\t4. Back\n\nOption: ");
    scanf("%d", &iOption);
    while((c = getchar()) != '\n' && c != EOF);
    /* printf("Your selected option: %i\n", iOption); */

    switch(iOption) {
        case 1: {printf("City_AddCityClientUI\n");
            UserInteractionAddCity(_LLC);
            return;
        }
        case 2: {printf("City_RemoveCityClientUI\n");
            UserInteractionRemoveCity(_LLC);
            return;
        }
        case 3: {
            printf("1. Edit name\n2. Edit latitude\n3. Edit longitude\n");

            while(1)
            {
                int editOption;
                scanf("%d", &editOption);
                while((c = getchar()) != '\n' && c != EOF);


                printf("Set the value: ");
                char* getValue = ClientUI_GetUserInputChar();


                printf("\nEnter your city: ");
                char* cityToEdit = ClientUI_GetUserInputChar();

                switch (editOption)
                {
                    case 1:
                        City_EditCity(_LLC, cityToEdit, getValue, NULL, NULL);
                        break;
                    case 2:
                        City_EditCity(_LLC, cityToEdit, NULL, getValue, NULL);
                        break;
                    case 3:
                        City_EditCity(_LLC, cityToEdit, NULL, NULL, getValue);
                        break;
                        
                    default:
                        printf("Not a valid edit option!\n");
                        continue;
                }
                free(getValue);
                free(cityToEdit);
                break;
            }

            return;
        }
        case 4: {printf("return to main.\n");
            return;
        }
        default: printf("No valid option selected.\n");
    }
    return;
}

int UserInteractionAddCity(LinkedListCities* _LLC){

    double lat;
    double lon;
    int c;

    printf("Enter new City name: ");
    char* newCityName = ClientUI_GetUserInputChar();

    printf("Enter latitude: ");
    scanf("%le", &lat);
    while((c = getchar()) != '\n' && c != EOF);

    printf("Enter longitude: ");
    scanf("%le", &lon);
    while((c = getchar()) != '\n' && c != EOF);

    City* OldCity = City_FindCity(_LLC, newCityName);
    if(OldCity != NULL){
        printf("New city name \"%s\" already exists!\n", newCityName);
        free(newCityName);
        return 1;
    }else
    {   
        int addCityErrCode =  City_AddCityToLinkedList(_LLC, newCityName, lat, lon, NULL);
        if (addCityErrCode != 0){
            free(newCityName);
            return -1;
        }
        char* strLat = doubleToString(lat);
        char* strLon = doubleToString(lon);
    
        /* City_SaveToJsonFile(newCityName, strLat, strLon); */
    
        free(newCityName);
        free(strLat);
        free(strLon);
    }

    return 0;
}

int UserInteractionRemoveCity(LinkedListCities* _LLC){
    
    printf("Enter city to remove: \n");
    char* removeName = ClientUI_GetUserInputChar();
    City* selectedCity = City_FindCity(_LLC, removeName);
    free(removeName);
    if(selectedCity == NULL){
        printf("No city by that name found. No city removed.\n");
        return 1;
    }
    else
    {
        City_RemoveCityFromLinkedList(_LLC, selectedCity);
    }
    return 0;
}

/* User input and returns City in one go. deprecated*/
int UserSelectCity(LinkedListCities* _LLC, City** _SelectedCity){
    
/* Get user input and format with FormatUserInput */
    char buffer[100];
    printf("Select a city: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    CaseFormatSwedish(buffer);

/*  return -1 = exit    return 0 = city match found     return 1 = no match     */
    City* current = _LLC->head;

    if (strcmp(buffer, "Exit") == 0 || strcmp(buffer, "Quit") == 0 || strcmp(buffer, "Q") == 0  )
        return -1;
    
    while (current != NULL) {
        if (strcmp(current->displayName, buffer) == 0)
        {   
            *_SelectedCity = current;
            /* printf("current: %s\n",current->DisplayName); */
            /* printf("find city display name: %s\n", (*_SelectedCity)->DisplayName); */
            return 0;
        }
        current = current->next;
    }
    return 1;
}