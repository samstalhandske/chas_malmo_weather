#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../mcore/json/cJSON/cJSON.h"
#include "../mcore/utils/CaseFormSwe.h"
#include "../mcore/utils/strdup.h"
#include "../city/City.h"


char* UserSelectCityChar(){
/* Get user input and format with FormatUserInput */
    char buffer[100];
    printf("Select a city: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    char* returnString = strdupWSL(buffer);
    CaseFormatSwedish(returnString);

    return returnString;
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