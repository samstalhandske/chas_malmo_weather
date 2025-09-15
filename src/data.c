#include <stdio.h>
#include <string.h>
#include "../include/data.h"

static struct cityDatabase cityArray[17];

void buildDatabase()
{    
    int count = 0;
    const char* ptr = cities;

    while (*ptr) 
    {
        char city[100];
        double lat, lon;
        int n = sscanf(ptr, "%99[^:]:%lf:%lf\n", city, &lat, &lon);
                
        if (n == 3) 
        {
            strcpy(cityArray[count].city, city);
            cityArray[count].lat = lat;
            cityArray[count].lon = lon;
            
            sprintf(cityArray[count].url, sizeof(cityArray[count].url),
                "https://api.open-meteo.com/v1/forecast?latitude=%.4lf&longitude=%.4lf&current_weather=true", lat, lon);
            count++;
        }
                    
        const char* next = strchr(ptr, '\n');
        if (!next) break;
        ptr = next + 1;
    }
}

void listCities()
{
    int count = sizeof(cityArray) / sizeof(cityArray[0]);
    for(int i = 0; i < count; i++)
    {
        printf("%i.%s\n", i+1, cityArray[i].name);
    }
}

int citySelect()
{
    int choice;

    printf("Enter a number to select city; ");
    scanf("%d", &choice);

    return choice;
}

void fetchUrl()
{
    selectedCity = citySelect();
    printf("%s", cityArray[selectedCity].url)
}