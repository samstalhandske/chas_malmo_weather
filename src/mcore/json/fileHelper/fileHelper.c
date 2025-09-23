#include <stdio.h>
#include <stdlib.h>
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
		
	char* JsonString = (char*)calloc(1, length + 1);

	if (JsonString == NULL) {
		fclose(file);
		fprintf(stderr, "Memory allocation failed\n");
		return NULL;
	}
	
	fread(JsonString, 1, length, file);
	JsonString[length] = '\0';
	fclose(file);
	
	cJSON* jsonParse = cJSON_Parse(JsonString);
	free(JsonString);
	
	return jsonParse;
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
	
	char* jsonObjectString = cJSON_Print(JSON_Object);
	fprintf(f, "%s\n", jsonObjectString);
	fclose(f);
	free(jsonObjectString);
	
	return 0;

}

int DoesFileExist(const char* fileName)
{
	FILE *file = fopen(fileName, "r");
	if (file) {
		fclose(file);
		return 1;
	} else {
		return 0;
	}
}