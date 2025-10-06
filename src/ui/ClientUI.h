#ifndef __CLIENTUI_H__
#define __CLIENTUI_H__


#include "../city/City.h"
#include "../city/LinkedListCity.h"

int UserSelectCity(LinkedListCities* _LLC, City** _SelectedCity);

char* ClientUI_GetUserInputChar();

void ClientUI_Options(LinkedListCities* _LLC);

int UserInteractionAddCity(LinkedListCities* _LLCPtr);

int UserInteractionRemoveCity(LinkedListCities* _LLC);


#endif