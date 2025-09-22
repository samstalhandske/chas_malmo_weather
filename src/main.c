#include <stdio.h>
#include <stdbool.h>
#include <data.h>
/* #include <http.h> */
#include <assert.h>
#include "mcore/json/fileHelper/fileHelper.h"

#include "mcore/http/http.h"
#include "mcore/json/json.h"
#include "mcore/json/Get_Weather_Report.h"

int main() {
	bool programShouldExit = false;
/*
	Http h;
	int errorCode = HttpInitialize(&h);
	assert(errorCode == 0);
*/	
	printf("Welcome to this weather app\n");
	buildDatabase();

	cJSON* my_JSON_Object = Read_JSON_From_File("file.json");
	if(my_JSON_Object == NULL){
		my_JSON_Object = cJSON_CreateObject();
	}
	printf("%s", cJSON_Print(my_JSON_Object));
	Write_JSON_To_File("file.json", my_JSON_Object);	

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
		Weather_Report* CurrentWeather = Get_Weather_Report((fetchCityName(choice)), fetchLatitude(choice), fetchLongitude(choice));
		assert(CurrentWeather != NULL);

		    printf("\n\tCity:\t\t%s\n", CurrentWeather->cityname);
			printf("\tTemperature:\t%i °C\n", CurrentWeather->temperature);
			printf("\tWindspeed:\t%.2f m/s\n", CurrentWeather->windspeed);
			printf("\tWeathercode:\t%i\n", CurrentWeather->weathercode);
			printf("\tDescription:\t%s\n", CurrentWeather->description);
			printf("\tTime:\t\t%lld\n\n", CurrentWeather->timestamp);
		/*
		Http_Response response = {0};
		int perErrorCode = Perform(&h, fetchUrl(choice), &response);
		printf("Perform, error-code: %i.\n", perErrorCode);

		if (perErrorCode != 0) {
			continue;
		}
		
		
		Http_Dispose_Response(&response);
		*/
	}

	/* Dispose(&h); */

	return 0;
}