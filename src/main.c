#include <stdio.h>
#include <stdbool.h>
#include <data.h>
#include <http.h>
#include <assert.h>

#include "mcore/http/http.h"
#include "mcore/json/json.h"

int main() {
	bool programShouldExit = false;

	Http h;
	int errorCode = HttpInitialize(&h);
	assert(errorCode == 0);
	
	printf("Welcome to this weather app\n");
	buildDatabase();

	FILE *cached_weather_file = fopen("weather.json", "w");
	assert(cached_weather_file != NULL);
	if(cached_weather_file == NULL) {
		/* Vi lyckades inte öppna. Dåligt! */
		printf("FIX THIS!\n");
		assert(false);
	}

	/* TODO: Hämta data från cachade väderfilen och lägg i den i vår databas. */

	while (programShouldExit == false) {
		listCities();
		printf("Enter a number to select city; ");

		int choice;
		int scanfResult = scanf("%d", &choice);
		
		
		if(choice < 1 || choice > 16 || scanfResult <=0) /* TODO: SS - Don't hardcode 16 here. */
		{
			printf("Invalid selection!\n"); 
			while ((getchar()) != '\n');
			continue;
		}

		Http_Response response = {0};
		int perErrorCode = Perform(&h, fetchUrl(choice), &response);
		printf("Perform, error-code: %i.\n", perErrorCode);

		if (perErrorCode != 0) {
			continue;
		}

		{ /* JSON Experiments*/
			cJSON *my_root_object = cJSON_CreateObject();
			cJSON *hej_string_object = cJSON_CreateString("Hej");
			
			cJSON_AddItemToObject(my_root_object, "name", hej_string_object);
			
			printf("Skriv detta till fil: %s\n", cJSON_Print(my_root_object));
			FILE *f = fopen("file.json", "w");
			if (f == NULL)
			{
				printf("Error opening file!\n");
				exit(1);
			}
			
			fprintf(f, cJSON_Print(my_root_object));
			fclose(f);
			
			FILE *file = fopen("file.json", "r");
			fseek(file, 0, SEEK_END);
			long length = ftell(file);
			rewind(file);
			
			char* JsonString = malloc(length + 1);
			if (JsonString == NULL) {
				fclose(file);
				fprintf(stderr, "Memory allocation failed\n");
				return;
			}
			
			fread(JsonString, 1, length, file);
			JsonString[length] = '\0';
			fclose(file);
			
			cJSON* CityArrayJSON = cJSON_Parse(JsonString);
			printf("This is file: %s\n", cJSON_Print(CityArrayJSON));
			free(JsonString);
			
			cJSON* root = cJSON_Parse(response.data);
			cJSON* current_weather_object = cJSON_GetObjectItem(root, "current_weather");
			cJSON* temperature_obj = cJSON_GetObjectItem(current_weather_object, "temperature");
			
			printf("Temperature is: %.1f.\n", cJSON_GetNumberValue(temperature_obj));
		}
		
		Http_Dispose_Response(&response);
	}

	Dispose(&h);

	return 0;
}