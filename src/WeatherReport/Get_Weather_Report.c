#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../mcore/json/cJSON/cJSON.h"
#include "../data.h"
#include "Get_Weather_Report.h"

#include "../mcore/http/http.h"

/* ######################### Local functions ######################### */
char* WMOInterpreter(int _WMOCode);
/* ######################### Local functions ######################### */

/* (PR) Saknar City object i main.c ? blir inget namn på staden just nu (City* _City)*/
/*
    char* cityname;         TODO (PR) Name of city, f.ex Malmö, Stockholm not yet implemented 
    double latitude;        the variable speaks for itself 
    double longitude;       what this variable does is not properly understood by anyone
    long long timestamp;    the time obtained from OpenMeteo API: YYYYMMDDHHMM
    double windspeed;       Windspeed is provided as m/s (API data is provided as km/h, APIData*1000/3600 resolves to m/s)
    int temperature;        rounded up or down from API Double, degrees celcius
    int weathercode;        int 0-99 that has a corresponding text description, resolved in function WMOInterpreter(weathercode);
    char* description;      verbose weather description from OpenMeteo docs: https://open-meteo.com/en/docs#weather_variable_documentation
*/

Weather_Report* Get_Weather_Report(char* _CityName, double _Latitude, double _Longitude){
    char api_url[256]; /* "https://api.open-meteo.com/v1/forecast?latitude=%lf&longitude=%lf&current_weather=true"; */

    snprintf(api_url, sizeof(api_url), "https://api.open-meteo.com/v1/forecast?latitude=%.4lf&longitude=%.4lf&current_weather=true", _Latitude, _Longitude);

    printf("api_url: %s\n", api_url);


    Weather_Report* New_Weather_Report = malloc(sizeof(Weather_Report));
    if (New_Weather_Report == NULL) {
        fprintf(stderr, "Failed to allocate memory for Weather_Report.\n");
        return NULL;
    }
    /* else printf("Get_Weather_Report>> New_Weather_Report memory alloc success\n"); */



    /* HTTP CALL */
    Http myHttp;
    Http_Response* Response = calloc(1, sizeof(Http_Response));

    int ErrCode = Http_Initialize(&myHttp);
    assert(ErrCode == 0);

    /* Http_Perform(myHttp, api_url, Response); */

    

    int perErrCode = Http_Perform(&myHttp, api_url, Response);
    if(perErrCode != 0){
        printf("Get_Weather_Report>> http-Perform, error-code: %i.\n", perErrCode);
        printf("Response->data content ( %i bytes): %s\n", Response->size, Response->data);
    }

    assert(perErrCode == 0);
    
    if (Response == NULL || Response->data == NULL || Response->size == 0) {
        fprintf(stderr, "Http_Perform returned success but response is empty.\n");
        Http_Dispose_Response(Response);
        /*free(Response);*/
        free(New_Weather_Report);
    return NULL;
}

    
    cJSON* JsonRoot = cJSON_Parse(Response->data);

    if(JsonRoot == NULL)
    {
        printf("ERROR PARSING JSON.\n");
        const char* error_ptr = cJSON_GetErrorPtr();
        if(error_ptr!= NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
            cJSON_Delete(JsonRoot);
            return NULL;
        }
    }

    cJSON* CurrentWeatherBlob = cJSON_GetObjectItemCaseSensitive(JsonRoot, "current_weather"); 
    
    cJSON* jsonTemp = cJSON_GetObjectItem(CurrentWeatherBlob, "temperature");
    cJSON* weathercode = cJSON_GetObjectItem(CurrentWeatherBlob, "weathercode");
    cJSON* windspeed = cJSON_GetObjectItem(CurrentWeatherBlob, "windspeed");
    cJSON* jsonTime = cJSON_GetObjectItem(CurrentWeatherBlob, "time");

/* // additional data
    cJSON *interval = cJSON_GetObjectItem(CurrentWeatherBlob, "interval");
    cJSON *winddirection = cJSON_GetObjectItem(CurrentWeatherBlob, "winddirection");
    cJSON *is_day = cJSON_GetObjectItem(CurrentWeatherBlob, "is_day");
*/
    /* printf("%s", cJSON_Print(jsonTime)); */
    int Temperature = jsonTemp->valuedouble + 0.5;
    float WindSpeed = windspeed->valuedouble *1000/3600;
    /* int Time = atof(cJSON_Print(jsonTime)); */
    long long TimeLong = StringTimeToLongLong(cJSON_Print(jsonTime));
    
    int WCode = atof(cJSON_Print(weathercode));
    char* WeatherVerbose = WMOInterpreter(WCode);

    New_Weather_Report->cityname = _CityName;
    New_Weather_Report->latitude = _Latitude;
    New_Weather_Report->longitude = _Longitude;
    New_Weather_Report->timestamp = TimeLong;
    New_Weather_Report->windspeed = WindSpeed;
    New_Weather_Report->temperature = Temperature;
    New_Weather_Report->weathercode = WCode;
    New_Weather_Report->description = WeatherVerbose;

    /*
    printf("\n\tCity:\t\t%s\n", _CityName);
    printf("\tTemperature:\t%i °C\n", Temperature);
    printf("\tWindspeed:\t%.2f m/s\n", WindSpeed);
    printf("\tWeathercode:\t%i\n", WCode);
    printf("\tDescription:\t%s\n", WeatherVerbose);
    printf("\tTime:\t\t%lld\n\n", TimeLong);
*/

    cJSON_Delete(JsonRoot);
    Http_Dispose_Response(Response);
    Http_Dispose(&myHttp);
    /*free(Response); */
    Response = NULL;
    return New_Weather_Report;
}


long long StringTimeToLongLong(char* _stringTime){
    long long year, month, day, hour, minute;

    sscanf(_stringTime, "\"%lld-%lld-%lldT%lld:%lld", &year, &month, &day, &hour, &minute);

    long long compact = year * 100000000 + month * 1000000 + day * 10000 + hour * 100 + minute;

    return compact;
}

char* WMOInterpreter(int _WMOCode){
    /*printf("WMO Code input: %i\n", _WMOCode); */

    switch(_WMOCode){
        case 0: return "Clear Sky";
        case 1: return "Mainly clear sky";
        case 2: return "Partly clouded sky";
        case 3: return "Overcast";

        case 45: return "Foggy";
        case 48: return "Foggy with depositing rime";

        case 51: return "Light drizzle";
        case 53: return "Moderate drizzle";
        case 55: return "Heavy drizzle";

        case 56: return "Light freezing drizzle";
        case 57: return "Dense freezing drizzle";

        case 61: return "Slight rain";
        case 63: return "Moderate rain";
        case 65: return "Heavy rain";

        case 66: return "Light freezing rain";
        case 67: return "Heavy freezing rain";

        case 71: return "Light snowfall";
        case 73: return "Moderate snowfall";
        case 75: return "Heavy snowfall";

        case 77: return "Snow grains";

        case 80: return "Light rain showers";
        case 81: return "Moderate rain showers";
        case 82: return "Violent rain showers";

        case 85: return "Light snow showers";
        case 86: return "Heavy snow showers";

        case 95: return "Thunderstorm";

        case 96: return "Thunderstorm with slight hail";
        case 99: return "Thunderstorm with heavy hail";

        default: return "error";
    }
}