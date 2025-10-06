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

    char* JsonString = calloc(1, length + 1);
    if (JsonString == NULL) {
        fclose(file);
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    fread(JsonString, 1, length, file);
    fclose(file);

    cJSON* root = cJSON_Parse(JsonString);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "JSON parse error before: %s\n", error_ptr);
        }
    }

    free(JsonString);  /* ✅ Free after parsing */
    return root;
}

/* returns 0 for success and -1 for failure */
int Write_JSON_To_File(const char* fileName, cJSON* JSON_Object)
{
	assert(JSON_Object != NULL);
	FILE *f = fopen(fileName, "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		return -1;
	}
	char* tempJsonStr = cJSON_Print(JSON_Object);


	fprintf(f, "%s\n", tempJsonStr);
	fclose(f);
    free(tempJsonStr);
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


char* Read_JSON_From_File_Return_char(const char* fileName)
{	
	printf("hello from inside Read_JSON_From_File_Return_char function\n");
    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        return NULL;
    }else printf("%s fine not NULL", fileName);

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
    fclose(file);
    JsonString[length] = '\0';
    printf("before end of Read_JSON_From_File_Return_char");
    /* free(JsonString); */  /* ✅ Free after parsing */
    return JsonString;
}

int DeleteFile(const char* fileName)
{
    if (remove(fileName) == 0) {
        return 0;
    } else {
        perror("Fel vid borttagning av fil");
        return -1; 
    }
}