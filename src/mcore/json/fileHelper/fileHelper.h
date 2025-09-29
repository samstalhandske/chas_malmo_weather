#ifndef FILEHELPER_H
#define FILEHELPER_H

#include "mcore/json/cJSON/cJSON.h"

cJSON* Read_JSON_From_File(const char* fileName);

int Write_JSON_To_File(const char* fileName, cJSON* JSON_Object);

int DoesFileExist(const char* fileName);

char* Read_JSON_From_File_Return_char(const char* fileName);

int DeleteFile(const char* fileName);


#endif