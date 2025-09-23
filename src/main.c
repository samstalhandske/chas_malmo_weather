#include <stdio.h>
#include <stdbool.h>
#include "data.h"
#include <assert.h>
#include "mcore/json/fileHelper/fileHelper.h"

#include "mcore/http/http.h"
#include "mcore/json/json.h"
#include "WeatherReport/Get_Weather_Report.h"

int main() {
	bool programShouldExit = false;

	buildDatabase();

	cJSON* my_JSON_Object = Read_JSON_From_File("file.json");
	if(my_JSON_Object == NULL){
		my_JSON_Object = cJSON_CreateObject();
	}
	printf("%s\n", cJSON_Print(my_JSON_Object));
	Write_JSON_To_File("file.json", my_JSON_Object);	
	/* TODO: Hämta data från cachade väderfilen och lägg i den i vår databas. */

	printf("Welcome to this weather app\n");

	while (programShouldExit == false) {
		listCities();
		printf("Enter a number to select city; ");

		int choice;
		int scanfResult = scanf("%d", &choice);
		
		if(choice < 1 || choice > 16 || scanfResult <= 0) /* TODO: SS - Don't hardcode 16 here. */
		{
			printf("Invalid selection!\n"); 
			while ((getchar()) != '\n');
			continue;
		}

		City* selectedCity = fetchCity(choice - 1);
		assert(selectedCity != NULL);

		Weather_Report* CurrentWeather = Get_Weather_Report(selectedCity->name, selectedCity->latitude, selectedCity->longitude);
		assert(CurrentWeather != NULL);

		printf("\n\tCity:\t\t%s\n", CurrentWeather->cityname);
		printf("\tTemperature:\t%i °C\n", CurrentWeather->temperature);
		printf("\tWindspeed:\t%.2f m/s\n", CurrentWeather->windspeed);
		printf("\tWeathercode:\t%i\n", CurrentWeather->weathercode);
		printf("\tDescription:\t%s\n", CurrentWeather->description);
		printf("\tTime:\t\t%lld\n\n", CurrentWeather->timestamp);
	}
	return 0;
}