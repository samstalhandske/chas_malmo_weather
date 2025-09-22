#ifndef MCORE_JSON
#define MCORE_JSON

/*
    TODO: SS - Add USE_CJSON or something to allow
    us to use other libraries, like Jansson.
*/
#include "cJSON/cJSON.h"

char* json_print(char* data);

#endif