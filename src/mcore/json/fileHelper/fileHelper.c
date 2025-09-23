#include <stdio.h>
#include <stdlib.h>
#include "../json.h"
#include <assert.h>
#include "fileHelper.h"

cJSON* Read_JSON_From_File(const char* fileName)
{
	FILE *file = fopen(fileName, "r");
	if(file == NULL){
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	rewind(file);
		
	char* JsonString = calloc(1, length + 1);
	if (JsonString == NULL) {
		fclose(file);
		fprintf(stderr, "Memory allocation failed\n");
		return NULL;
	}
	
	fread(JsonString, 1, length, file);
	JsonString[length] = '\0';
	fclose(file);	
	
	return cJSON_Parse(JsonString);
}
	
int Write_JSON_To_File(const char* fileName, cJSON* JSON_Object)
{
	assert(JSON_Object != NULL);
	FILE *f = fopen(fileName, "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		return -1;
	}
	
	fprintf(f, "%s\n", cJSON_Print(JSON_Object));
	fclose(f);
	
	return 0;

}

    