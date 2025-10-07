#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "UserInteractions.h"
#include "../mcore/json/cJSON/cJSON.h"
#include "../mcore/utils/CaseFormSwe.h"
#include "../mcore/utils/strdup.h"
#include "../city/City.h"
#include "../mcore/utils/dtos.h"
#include "../mcore/json/fileHelper/fileHelper.h"

#include <Windows.h>

#include <locale.h>

char* UserSelectCityChar() {

    char buffer[100];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return NULL;
    }
    buffer[strcspn(buffer, "\n")] = '\0';

    char* returnString = strdup(buffer);
    // CaseFormatSwedish(returnString);

    for (size_t i = 0; i < strlen(returnString); i++) {
        printf("%02X ", (unsigned char)returnString[i]);
    }
    puts("");


    return returnString; // return user input, not hardcoded string
}

void UserSelectOptions(LinkedListCities* _LLC){

    int iOption;
    int c;
    printf("\n\t1. Add City\n\t2. Remove City\n\t3. Edit city\n\t4. Back\n\nMake your choice: ");
    scanf("%d", &iOption);
    while((c = getchar()) != '\n' && c != EOF);
    printf("Your selected option: %i\n", iOption);

    switch(iOption) {
        case 1: {printf("City_AddCityClientUI\n");
            int errCode = UserInteractionAddCity(_LLC);
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
                char* getValue = UserSelectCityChar();


                printf("\nEnter your city: ");
                char* cityToEdit = UserSelectCityChar();

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



                break;
            }

            return;
        }
        case 4: {printf("return to main\n");
            return;
        }
        default: printf("No valid option selected\n");
    }
    return;
}

int UserInteractionAddCity(LinkedListCities* _LLC){

    double lat;
    double lon;
    int c;

    printf("Enter new City name: ");
    char* newCityName = UserSelectCityChar();

    printf("Enter latitude: ");
    scanf("%le", &lat);
    while((c = getchar()) != '\n' && c != EOF);

    printf("Enter longitude: ");
    scanf("%le", &lon);
    while((c = getchar()) != '\n' && c != EOF);

    City* OldCity = City_FindCity(_LLC, newCityName);
    if(OldCity != NULL){
        printf("New city name \"%s\" already exists!\n", newCityName);
        return 1;
    }else
    {   
        int addCityErrCode =  City_AddCityToLinkedList(_LLC, newCityName, lat, lon, NULL);
        if (addCityErrCode != 0){
            return -1;
        }
    }
    char* strLat = doubleToString(lat);
    char* strLon = doubleToString(lon);

    City_SaveToJsonFile(newCityName, strLat, strLon);

    free(strLat);
    free(strLon);

    return 0;
}

int UserInteractionRemoveCity(LinkedListCities* _LLC){
    
    printf("Enter city to remove: \n");
    char* removeName = UserSelectCityChar();

    City* selectedCity = City_FindCity(_LLC, removeName);
    if(selectedCity == NULL){
        printf("City \"%s\" not found.\n", removeName);
        return 1;
    }
    else
    {
        City_RemoveCityFromLinkedList(_LLC, selectedCity);
    }

    
    return 0;
}

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