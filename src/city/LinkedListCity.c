#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

#include "LinkedListCity.h"
#include "City.h"
#include "../mcore/utils/CreateJsonCacheFilePath.h"
#include "../weather/Weather.h"
#include "../mcore/json/cJSON/cJSON.h"
#include "../mcore/utils/CaseFormSwe.h"
#include "../mcore/utils/strdup.h"
#include "../mcore/utils/CreateDirectory.h"
#include "../mcore/json/fileHelper/fileHelper.h"


/*  Bootstrapper data for stored cities.    */
const char* cities =    "Stockholm:59.3293:18.0686\n" "Göteborg:57.7089:11.9746\n" "Malmö:55.6050:13.0038\n" 
                        "Uppsala:59.8586:17.6389\n" "Västerås:59.6099:16.5448\n" "Örebro:59.2741:15.2066\n" 
                        "Linköping:58.4109:15.6216\n" "Helsingborg:56.0465:12.6945\n" "Jönköping:57.7815:14.1562\n" 
                        "Norrköping:58.5877:16.1924\n" "Lund:55.7047:13.1910\n" "Gävle:60.6749:17.1413\n" 
                        "Sundsvall:62.3908:17.3069\n" "Umeå:63.8258:20.2630\n" "Luleå:65.5848:22.1567\n" 
                        "Kiruna:67.8558:20.2253\n";


/* moved from City.c/.h */

int City_ParseDefaultCityString(LinkedListCities* _LLC, const char* _BootstrapString);

int City_ParseCachedCities(LinkedListCities* _LLC, const char* dir_path);


/*  Initializes LinkedList and directories */
int City_InitializeCitySystem(LinkedListCities* _LLC){

    memset(_LLC, 0, sizeof(LinkedListCities));

    /* cache directory for city information */
    DirectoryCreate("cachedcity");
    /* cache directory for http json data */
    DirectoryCreate("cachedreports");
    
    int parseErrCode = City_ParseDefaultCityString(_LLC, cities);
    if(parseErrCode == CITY_PARSE_FAIL){
        LLC_DestroyLinkedListCities(_LLC);
        return CITY_INIT_FAIL;
    }
    parseErrCode = City_ParseCachedCities(_LLC, "cachedcity");
    if(parseErrCode == CITY_PARSE_FAIL){
        LLC_DestroyLinkedListCities(_LLC);
        return CITY_INIT_FAIL;
    }
    return CITY_INIT_OK;
}

/* Parse cities from bootstrap string */
int City_ParseDefaultCityString(LinkedListCities* _LLC, const char* _BootstrapString){

    /* copy of const char* because we will replace delimiters */
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

                if(City_AddCityToLinkedList(_LLC, displayName, atof(latitudestring), atof(longitudestring), NULL) == CITY_ADDTOLINKEDLIST_FAIL)
                    return CITY_PARSE_FAIL;

				displayName = NULL;
				latitudestring = NULL;
				longitudestring = NULL;
			}
		}

		ptr++;

	} while (*(ptr) != '\0');
	
    free(DataCopy);
    DataCopy = NULL;
    ptr = NULL;
    return CITY_PARSE_OK;
}


/* reads stored city json data and adds nodes to LinkedList */
int City_ParseCachedCities(LinkedListCities* _LLC, const char* dir_path){

    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("opendir failed");
        return CITY_PARSE_FAIL;
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
            return CITY_PARSE_FAIL;
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
        double latitude = (cJSON_GetObjectItem(jsonRoot, "latitude"))->valuedouble;
        double longitude = (cJSON_GetObjectItem(jsonRoot, "longitude"))->valuedouble;
        
        
        /* test if city already in list */
        City* City = City_FindCity(_LLC, displayName);
        if (City == NULL){
            City_AddCityToLinkedList(_LLC, displayName, latitude, longitude, NULL);
        }else{ 
        /* Already in list // Do nothing to not make list  doubles */
        }
        cJSON_Delete(jsonRoot);
        free(displayName);
    }
    closedir(dir);
    return CITY_PARSE_OK;
}

/* get city by name, returns NULL in case of no city found*/
City* City_FindCity(LinkedListCities* _LLC, const char* _Name) {
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

/* Adds a city to the LinkedListCities and saves city as json data */
int City_AddCityToLinkedList(LinkedListCities* _LLC, char* _DisplayName, double _Latitude, double _Longitude, City** _CityPtr) {
    City* NewCity = (City*)malloc(sizeof(City));
    if(NewCity == NULL){
        printf("Memory allocation for NewCity failed!\n");
        return CITY_ADDTOLINKEDLIST_FAIL;
    }

    NewCity->displayName = strdup(_DisplayName);
    NewCity->latitude = _Latitude;
    NewCity->longitude = _Longitude;
    NewCity->WeatherData = NULL;
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
    
    /* save to cachedcity/city_lat_lon.json */
    City_SaveToJsonFile(NewCity);

    return CITY_ADDTOLINKEDLIST_OK;
}


/* removes City from LinkedListCities */
int City_RemoveCityFromLinkedList(LinkedListCities* _LLC, City* _City)
{
    /* Free cached report WeatherData object */
    if(_City->WeatherData != NULL)
    Weather_DestroyReport(_City->WeatherData);
    
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
    
    /* construct city cached file name*/
    char* jsonFilename = ConstructCityCacheFilePath(_City);

    DeleteFile(jsonFilename); 
    free(jsonFilename);
    free(_City->displayName);
    free(_City);

    return 0;
}




/* Print displayName of nodes in LinkedListCities. Displays city names in a 5 row grid-like fashion*/
int LLC_DisplayLinkedListCities(LinkedListCities* _LLC)
{
	City* current = _LLC->head;
	if(current == NULL)
	{
		return LLC_DISPLAY_NOCITIESFOUND;
	}
    int i = 1; /* use an integer to print a select number of cities in a grid like view */
	do
	{
		printf("%s\t", current->displayName);
        if(strlen(current->displayName)< 8) printf("\t"); /* extra tab when displayName is too short */
		current = current->next;
        if (i % 5 == 0) printf("\n"); /* new line after 5 cities */
        i++;
	} while (current != NULL);
    printf("\n");
    return LLC_DISPLAY_OK;
}

/* Delete all linked list nodes.  Run at end of program execution to free memory. Calls Weather_DestroyReport to free any stored reports */
void LLC_DestroyLinkedListCities(LinkedListCities* _LLC) {
    City* current = _LLC->head;
    int i = 0;
    while (current != NULL) {
        City* next = current->next;

        /* if there is a weather report, free it! */
        if(current->WeatherData != NULL)
            Weather_DestroyReport(current->WeatherData);
        
        free(current->displayName); /* free allocd city name*/

        /* Free the node itself */
        free(current);

        current = next;
        i++;
    }
    _LLC->head = NULL;
    _LLC->tail = NULL;
}

/* TODO: Fix writing to JSON as NUMBER instead of STRING. make name change also impact LinkedList */
/* Edits a City's values and saves it */
int City_EditCity(LinkedListCities* _LLC, const char* _CityName, const char* _NewName, const char* _Latitude, const char* _Longitude)
{
    if (_LLC == NULL)
    {
        printf("LinkedListCities is NULL!\n");
        return -1;
    }

    if (_CityName == NULL)
    {
        printf("_CityName is NULL\n");
        return -1;
    }

    City* city = City_FindCity(_LLC, _CityName);

    if (city == NULL)
    {
        printf("City to edit could not be found!\n");
        return -1;
    }

    const size_t jsonExtensionSize = 5;
    const size_t cachedCityName = strlen("cachedcity/");

    size_t oldFileNameSize = cachedCityName + jsonExtensionSize + 1 + strlen(city->displayName) + 20; 
    
    char oldFileName[oldFileNameSize];
    sprintf(oldFileName, "cachedcity/%s%.4f%.4f.json", city->displayName, city->latitude, city->longitude);

    if (_NewName)
    {
        /* 20 is the size for _Latitude and _Longitude, 5 for json extension  */
        /* + 1 for new line  */
    
        sprintf(oldFileName, "cachedcity/%s%.4f%.4f.json", city->displayName, city->latitude, city->longitude);
        printf("%s\n", oldFileName);

        if (DoesFileExist(oldFileName))
        {
            cJSON* fileRead = Read_JSON_From_File(oldFileName);
            DeleteFile(oldFileName);

            cJSON_ReplaceItemInObjectCaseSensitive(fileRead, "displayName", cJSON_CreateString(_NewName));

            size_t sizeToAdd = 0;
            sizeToAdd += cachedCityName + strlen(_NewName) + 20 + jsonExtensionSize + 1; 

            city->displayName = realloc(city->displayName, strlen(city->displayName) + 1);
            city->displayName = strdup(_NewName);

            char newFileName[sizeToAdd];
            sprintf(newFileName, "cachedcity/%s%.4f%.4f.json", _NewName, city->latitude, city->longitude);
            Write_JSON_To_File(newFileName, fileRead);
            cJSON_Delete(fileRead);
        }
    }                                                               

    if (_Latitude)
    {
        if (DoesFileExist(oldFileName))
        {
            cJSON* fileRead = Read_JSON_From_File(oldFileName);
            DeleteFile(oldFileName);

            cJSON_ReplaceItemInObjectCaseSensitive(fileRead, "latitude", cJSON_CreateString(_Latitude));

            char buffer[oldFileNameSize];

            double latToDouble = atof(_Latitude);
            sprintf(buffer, "cachedcity/%s%.4f%.4f.json", city->displayName, latToDouble, city->longitude);
            city->latitude = latToDouble;

            Write_JSON_To_File(buffer, fileRead);
            cJSON_Delete(fileRead);
        }
    }


    if (_Longitude)
    {
        if (DoesFileExist(oldFileName))
        {
            cJSON* fileRead = Read_JSON_From_File(oldFileName);
            DeleteFile(oldFileName);

            cJSON_ReplaceItemInObjectCaseSensitive(fileRead, "longitude", cJSON_CreateString(_Longitude));

            char buffer[oldFileNameSize];

            double lonToDouble = atof(_Longitude);
            sprintf(buffer, "cachedcity/%s%.4f%.4f.json", city->displayName, city->latitude, lonToDouble);
            city->longitude = lonToDouble;


            Write_JSON_To_File(buffer, fileRead);
            cJSON_Delete(fileRead);
        }
    }

    return 0;
}
