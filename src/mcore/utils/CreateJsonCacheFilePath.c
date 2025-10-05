#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dtos.h" /* double to string, free after use */
#include "../../city/City.h"

/* cached city file path constructor */
char* ConstructCityCacheFilePath(City* _City){


    char* latStr = doubleToString(_City->latitude);
    char* lonStr = doubleToString(_City->longitude);

    size_t memoryNeeded = strlen(_City->displayName) + strlen(latStr) + strlen(lonStr) + strlen("cachedcity/") + strlen(".json") + 1;
    char* jsonFilePath = malloc(memoryNeeded); /* +4 for ".json" and +1 for null terminator */
    sprintf(jsonFilePath, "cachedcity/%s%.7s%.7s.json", _City->displayName, latStr, lonStr);

    free(latStr);
    free(lonStr);
    return jsonFilePath;
}

/* cached report file path constructor */
char* ConstructReportCacheFilePath(City* _City){


    char* latStr = doubleToString(_City->latitude);
    char* lonStr = doubleToString(_City->longitude);

    size_t memoryNeeded = strlen(_City->displayName) + strlen(latStr) + strlen(lonStr) + strlen("cachedreports/") + strlen(".json") + 1;
    char* jsonFilePath = malloc(memoryNeeded); /* +4 for ".json" and +1 for null terminator */
    sprintf(jsonFilePath, "cachedreports/%s%.7s%.7s.json", _City->displayName, latStr, lonStr);

    free(latStr);
    free(lonStr);
    return jsonFilePath;
}