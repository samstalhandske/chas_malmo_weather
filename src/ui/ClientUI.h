#ifndef __CLIENTUI_H__
#define __CLIENTUI_H__


#include "../city/LinkedListCity.h"

char* ClientUI_GetUserInputChar();

void ClientUI_Options(LinkedListCities* _LLC);

int UserInteractionAddCity(LinkedListCities* _LLCPtr);

int UserInteractionRemoveCity(LinkedListCities* _LLC);

int Client_Run();


#endif