#include <stdlib.h>
#include <stdio.h>

#include "City.h"
#include "../mcore/json/cJSON/cJSON.h"
#include "../mcore/utils/CreateJsonCacheFilePath.h"


/* saves a single City data to json file */
int City_SaveToJsonFile(City* _City){    

    char* jsonFilePath = ConstructCityCacheFilePath(_City);

    FILE* file = fopen(jsonFilePath, "w");
    if (file == NULL){
        printf("Error opening file %s", jsonFilePath);
        perror("Error opening file");
        return CITY_SAVETOJSON_FAIL;
    }
    cJSON* jsonRoot = cJSON_CreateObject();

    cJSON_AddStringToObject(jsonRoot, "displayName", _City->displayName);
    cJSON_AddNumberToObject(jsonRoot, "latitude", _City->latitude);
    cJSON_AddNumberToObject(jsonRoot, "longitude", _City->longitude);

    char* jsonString = cJSON_Print(jsonRoot);

    fprintf(file, "%s\n", jsonString);
    fclose(file);

    cJSON_Delete(jsonRoot);
    free(jsonString);
    free(jsonFilePath);

    return CITY_SAVETOJSON_OK;
}

