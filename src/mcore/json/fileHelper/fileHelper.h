#ifndef FILEHELPER_H
#define FILEHELPER_H
#include "../json.h"

cJSON* Read_JSON_From_File(const char* fileName);

int Write_JSON_To_File(const char* fileName, cJSON* JSON_Object);

#endif