#include "json.h"

#include <stdio.h>

char* json_print(char* data) {
    cJSON* root = cJSON_Parse(data);

    /* cJSON* timezone_item = cJSON_GetObjectItem(root, "timezone"); */
    /* printf("Timezone: %s\n", cJSON_GetStringValue(timezone_item)); */
    
    return cJSON_Print(root);
}