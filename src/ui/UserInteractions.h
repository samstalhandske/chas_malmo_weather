#ifndef USERINTERACTIONS_H
#define USERINTERACTIONS_H


#include "../city/City.h"

int UserSelectCity(LinkedListCities* _LLC, City** _SelectedCity);

char* UserSelectCityChar();

void UserSelectOptions(LinkedListCities* _LLC);

int UserInteractionAddCity(LinkedListCities* _LLCPtr);


#endif