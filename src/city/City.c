#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


#include "LinkedListCity.h"
#include "City.h"
#include "../weather/Weather.h"
#include "../mcore/json/cJSON/cJSON.h"
#include "../mcore/utils/CaseFormSwe.h"
#include "../mcore/utils/strdup.h"
#include "../mcore/utils/CreateDirectory.h"
#include "../mcore/json/fileHelper/fileHelper.h"
#include "../mcore/utils/CreateJsonCacheFilePath.h"

#include "../mcore/json/fileHelper/fileHelper.h"


/* LOCAL FUNCTIONS */

/* LOCAL FUNCTIONS */

/* enums moved to .h*/






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

