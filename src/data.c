#include <stdio.h>
#include <string.h>
#include "data.h"

static cityDatabase cityArray[17];
static const char* cities = "Stockholm:59.3293:18.0686\n" "Göteborg:57.7089:11.9746\n" "Malmö:55.6050:13.0038\n" "Uppsala:59.8586:17.6389\n" "Västerås:59.6099:16.5448\n" "Örebro:59.2741:15.2066\n" "Linköping:58.4109:15.6216\n" "Helsingborg:56.0465:12.6945\n" "Jönköping:57.7815:14.1562\n" "Norrköping:58.5877:16.1924\n" "Lund:55.7047:13.1910\n" "Gävle:60.6749:17.1413\n" "Sundsvall:62.3908:17.3069\n" "Umeå:63.8258:20.2630\n" "Luleå:65.5848:22.1567\n" "Kiruna:67.8558:20.2253\n";


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
            strcpy(cityArray[count].name, city);
            cityArray[count].latitude = lat;
            cityArray[count].longitude = lon;
            
            sprintf(cityArray[count].URL,
                "https://api.open-meteo.com/v1/forecast?latitude=%lf&longitude=%lf&current_weather=true", lat, lon);
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
    int i = 0;
    for(;i < count; i++)
    {
        printf("%i.%s\n", i+1, cityArray[i].name);
    }
}

int citySelect()
{
    int choice;

    listCities();
    printf("Enter a number to select city; ");
    scanf("%d", &choice);

    return choice;
}

void fetchUrl()
{
    int selectedCity = citySelect();
    printf("%s", cityArray[selectedCity-1].URL);
}