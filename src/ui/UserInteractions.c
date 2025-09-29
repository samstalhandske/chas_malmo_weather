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
    printf("\nOptions for options, Exit (Quit or Q) to exit.\nSelect a city: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    char* returnString = strdup(buffer);
    CaseFormatSwedish(returnString);

    return returnString;
}

void UserSelectOptions(){

    int iOption;
    int c;
    printf("\n\t1. Add City\n\t2. Remove City\n\t3. Edit city\n\t4. Back\n\nMake your choice: ");
    scanf("%d", &iOption);
    while((c = getchar()) != '\n' && c != EOF);
    printf("Your selected option: %i\n", iOption);

    switch(iOption) {
        case 1: {printf("City_AddCityClientUI\n");
            return;
        }
        case 2: {printf("City_RemoveCityClientUI\n");
            return;
        }
        case 3: {printf("City_EditCityClientUI\n");
            return;
        }
        case 4: {printf("return to main\n");
            return;
        }
        default: printf("No valid option selected\n");
    }
    return;
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