#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "City.h"
#include "../mcore/json/cJSON/cJSON.h"
#include "../mcore/utils/CaseFormSwe.h"
#include "../mcore/utils/strdup.h"
#include "../mcore/utils/CreateDirectory.h"
#include "../mcore/json/fileHelper/fileHelper.h"


/* LOCAL FUNCTIONS */

int City_ParseDefaultCityString(LinkedListCities* _LLC, const char* _BootstrapString);
int City_SaveToJsonFile(const char* _Name, const char* _Latitude, const char* _Longitude);
int City_ParseCachedCities(LinkedListCities* _LLC, const char* dir_path);

/* LOCAL FUNCTIONS */

/* enums for CITY functions @(SS) Vi behöver ha lite ENUM input på det här*/
typedef enum {
    CITY_PARSE_OK = 0,
    CITY_PARSE_FAIL = 1,
    CITY_ADDTOLINKEDLIST_OK = 0,
    CITY_ADDTOLINKEDLIST_FAIL = 1,
    CITY_INIT_OK = 0,
    CITY_INIT_FAIL = -1
} City_ReturnENUMS;

const char* cities =    "Stockholm:59.3293:18.0686\n" "Göteborg:57.7089:11.9746\n" "Malmö:55.6050:13.0038\n" 
                        "Uppsala:59.8586:17.6389\n" "Västerås:59.6099:16.5448\n" "Örebro:59.2741:15.2066\n" 
                        "Linköping:58.4109:15.6216\n" "Helsingborg:56.0465:12.6945\n" "Jönköping:57.7815:14.1562\n" 
                        "Norrköping:58.5877:16.1924\n" "Lund:55.7047:13.1910\n" "Gävle:60.6749:17.1413\n" 
                        "Sundsvall:62.3908:17.3069\n" "Umeå:63.8258:20.2630\n" "Luleå:65.5848:22.1567\n" 
                        "Kiruna:67.8558:20.2253\n";

int City_InitializeCitySystem(LinkedListCities* _LLC){

    memset(_LLC, 0, sizeof(LinkedListCities));
    CreateDirectory("cachedcity");
    CreateDirectory("cachedreports");
    
    int parseErrCode = City_ParseDefaultCityString(_LLC, cities);
    assert(parseErrCode == 0);

    parseErrCode = City_ParseCachedCities(_LLC, "cachedcity");
    assert(parseErrCode == 0);

    return CITY_INIT_OK;
}

/* Parse cities from hard coded string */
int City_ParseDefaultCityString(LinkedListCities* _LLC, const char* _BootstrapString){
    printf("Started parsing city list.\n");
    char* DataCopy = strdup(_BootstrapString);

    if(DataCopy == NULL){
        printf("Failed to allocate memory for list.\n");
        free(DataCopy);
        return CITY_PARSE_FAIL;
    }

	char* ptr = DataCopy;
	char* displayName = NULL;
	char* latitudestring = NULL;
	char* longitudestring = NULL;

	do
	{
		if(displayName == NULL)
		{
			displayName = ptr;
		}
		else if(latitudestring == NULL)
		{
			if(*(ptr) == ':')
			{
				latitudestring = ptr + 1;
				*(ptr) = '\0';
			}
		}
		else if(longitudestring == NULL)
		{
			if(*(ptr) == ':')
			{
				longitudestring = ptr + 1;
				*(ptr) = '\0';
			}
		}
		else
		{
			if(*(ptr) == '\n')
			{
				*(ptr) = '\0';

				/* printf("City: <%s>, Latitude: <%s>, Longitude: <%s>\n", displayName, latitudestring, longitudestring); */
                City_AddCityToLinkedList(_LLC, displayName, atof(latitudestring), atof(longitudestring), NULL);

                City_SaveToJsonFile(displayName, latitudestring, longitudestring);

				displayName = NULL;
				latitudestring = NULL;
				longitudestring = NULL;
			}
		}

		ptr++;

	} while (*(ptr) != '\0');
	
	printf("Finished parsing city list.\n");
    free(DataCopy);
    ptr = NULL;
    return CITY_PARSE_OK;
}

/* Adds a city to the LinkedListCities */
int City_AddCityToLinkedList(LinkedListCities* _LLC, char* _DisplayName, double _Latitude, double _Longitude, City** _CityPtr) {
    City* NewCity = (City*)malloc(sizeof(City));
    if(NewCity == NULL){
        printf("Memory allocation for NewCity failed!\n");
        return CITY_ADDTOLINKEDLIST_FAIL;
    }

    NewCity->displayName = strdup(_DisplayName);
    NewCity->latitude = _Latitude;
    NewCity->longitude = _Longitude;

    NewCity->prev = NULL;
    NewCity->next = NULL;

    if(_LLC->tail == NULL){
        _LLC->head = NewCity;
        _LLC->tail = NewCity;
    }
    else{
        NewCity->prev = _LLC->tail;
        _LLC->tail->next = NewCity;
        _LLC->tail = NewCity;
    }

    if(_CityPtr != NULL){
        *(_CityPtr) = NewCity;
    }
    return CITY_ADDTOLINKEDLIST_OK;
}

/* removes City from LinkedListCities */
int City_RemoveCityFromLinkedList(LinkedListCities* _LLC, City* _City)
{
    if(_City->next == NULL && _City->prev == NULL)
    {
        _LLC->head = NULL;
        _LLC->tail = NULL;
    }
    else if(_City->prev == NULL)
    {
        _LLC->head = _City->next;
        _City->next->prev = NULL;
    }
    else if(_City->next == NULL)
    {
        _LLC->tail = _City->prev;
        _City->prev->next = NULL;
    }
    else
    {
        _City->prev->next = _City->next;
        _City->next->prev = _City->prev;
    }
    size_t memoryNeeded = strlen(_City->displayName) + sizeof(_City->latitude) + sizeof(_City->longitude) + strlen("cachedreports/") + strlen(".json") + 1;
    char* JsonFileName = malloc(memoryNeeded); /* +4 for ".json" and +1 for null terminator */
    sprintf(JsonFileName, "cachedcity/%s%.4f%.4f.json", _City->displayName, _City->latitude, _City->longitude);
    DeleteFile(JsonFileName); 
    free(JsonFileName);
    free(_City);

    return 0;
}

/* Print objects in LinkedListCities */
void City_DisplayLinkedListCities(LinkedListCities* _LLC)
{
	City* current = _LLC->head;
	if(current == NULL)
	{
		printf("No cities to print\n");
		return;
	}
    int i = 1;
	do
	{
		printf("%s\t", current->displayName);
        if(strlen(current->displayName)< 8) printf("\t");
		current = current->next;
        if (i % 5 == 0) printf("\n");
        i++;
	} while (current != NULL);
    printf("\n");
}

/* get city by name, returns NULL in case of no city found*/
City* City_FindCity(LinkedListCities* _LLC, char* _Name) {
    City* current = _LLC->head;

    while (current != NULL) {
        if (strcmp(current->displayName, _Name) == 0)
        {   
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int City_SaveToJsonFile(const char* _Name, const char* _Latitude, const char* _Longitude){
    size_t memoryNeeded = strlen(_Name) + strlen("cachedcity/") + strlen(".json") + 1;
    char* jsonFilePath = malloc(memoryNeeded); /* +4 for ".json" and +1 for null terminator */
    sprintf(jsonFilePath, "cachedcity/%s.json", _Name);

    FILE* file = fopen(jsonFilePath, "w");
    if (file == NULL){
        printf("Error opening file %s", jsonFilePath);
        perror("Error opening file");
        return -1;
    }
    cJSON* jsonRoot = cJSON_CreateObject();

    cJSON_AddStringToObject(jsonRoot, "displayName", _Name);
    cJSON_AddStringToObject(jsonRoot, "latitude", _Latitude);
    cJSON_AddStringToObject(jsonRoot, "longitude", _Longitude);

    char* jsonString = cJSON_Print(jsonRoot);

    fprintf(file, "%s\n", jsonString);
    fclose(file);

    cJSON_Delete(jsonRoot);
    free(jsonString);
    free(jsonFilePath);

    return 0;
}

int City_ParseCachedCities(LinkedListCities* _LLC, const char* dir_path){

    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("opendir failed");
        return -1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        /* Skip "." and ".." */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        /* Build full path */
        char full_path[1024];
        sprintf(full_path, "%s/%s", dir_path, entry->d_name);

        /* Check if it's a regular file */
        struct stat st;
        if (stat(full_path, &st) == -1 || !S_ISREG(st.st_mode))
            continue;


        FILE* file = fopen(full_path, "r");
        if (file == NULL){
            printf("Error opening file %s", full_path);
            perror("Error opening file");
            return -1;
        }
        
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        rewind(file);

        char* JsonString = malloc(length +1);
        if (JsonString == NULL) {
            fclose(file);
            fprintf(stderr, "Memory allocation failed\n");
            return -1;
        }
        
        fread(JsonString, 1, length, file);
        JsonString[length] = '\0';
        fclose(file);

        cJSON* jsonRoot = cJSON_Parse(JsonString);
        free(JsonString);

        char* displayName = strdup(cJSON_GetObjectItem(jsonRoot, "displayName")->valuestring);
        char* latStr = strdup(cJSON_GetObjectItem(jsonRoot, "latitude")->valuestring);
        double latitude =  atof(latStr);
        free(latStr);
        char* lonStr = strdup(cJSON_GetObjectItem(jsonRoot, "longitude")->valuestring);
        double longitude = atof(lonStr);
        free(lonStr);
        
        /* test if city already in list */
        City* City = City_FindCity(_LLC, displayName);
        if (City == NULL){
            City_AddCityToLinkedList(_LLC, displayName, latitude, longitude, NULL);
        }else{ 
        /* printf("City already in list\n"); */
        }
        cJSON_Delete(jsonRoot);
        free(displayName);
    }
    closedir(dir);
    return 0;
}

void City_DestroyLinkedListCities(LinkedListCities* _LLC) {
    City* current = _LLC->head;
    int i = 0;
    while (current != NULL) {
        City* next = current->next;
        /* printf("Freeing city: %s\n", current->displayName); */
        /* Free any dynamically allocated fields */
        free(current->displayName);

        /* Free the node itself */
        free(current);

        current = next;
        i++;
    }
    /* printf("Total freed cities: %i\n", i); */
    _LLC->head = NULL;
    _LLC->tail = NULL;
}


/* ############################# OLD FUNCTIONS #############################*/

/*
void buildDatabase()
{    
    int count = 0;
    const char* ptr = cities;

    while (*ptr) 
    {
        char name[100];
        double lat, lon;
        int n = sscanf(ptr, "%99[^:]:%lf:%lf\n", name, &lat, &lon);

        if (n == 3) 
        {
            City* city = &cityArray[count];
            
            strcpy(city->name, name);
            city->latitude = lat;
            city->longitude = lon;
            
            snprintf(&city->URL[0], sizeof(city->URL),
                "https://api.open-meteo.com/v1/forecast?latitude=%lf&longitude=%lf&current_weather=true", lat, lon);
            count++;
        }
                    
        const char* next = strchr(ptr, '\n');
        if (!next) break;
        ptr = next + 1;
    }
}

void listCities()
{
    int count = sizeof(cityArray) / sizeof(cityArray[0]);
    int i = 0;
    for(;i < count - 1; i++)
    {
        printf("%i. %s\n", i+1, cityArray[i].name);
    }
}

City* fetchCity(int selectedCityIndex) {
    if(selectedCityIndex < 0) {
        return NULL;
    }
    if(selectedCityIndex >= sizeof(cityArray) / sizeof(cityArray[0])) {
        return NULL;
    }

    return &cityArray[selectedCityIndex];
}
    */