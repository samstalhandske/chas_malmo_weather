#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../mcore/json/cJSON/cJSON.h"
#include "../city/City.h"
#include "../mcore/utils/sttll.h"
#include "../mcore/json/fileHelper/fileHelper.h"
#include "Weather.h"
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>


#include "../mcore/http/http.h"

/* ######################### Local functions ######################### */
cJSON* Weather_CheckCache(char* _FileName);
int Weather_MakeHTTPCall(char* _Url, cJSON** _JsonRootPtr);
int Weather_CreateReport(cJSON* _JsonRoot, WeatherReport* _NewWeatherReport, char* _CityName, double _Latitude, double _Longitude);
char* Weather_WindDirectionInterpreter(int _WindDirection);
char* WMOInterpreter(int _WMOCode);
/* ######################### Local functions ######################### */

/*
    char* cityname;         Name of city
    double latitude;        the variable speaks for itself 
    double longitude;       the variable speaks for itself
    long long timestamp;    the time obtained from OpenMeteo API: YYYYMMDDHHMM
    double windspeed;       Windspeed is provided as m/s (API data is provided as km/h, APIData*1000/3600 resolves to m/s)
    int temperature;        rounded up or down from API Double, degrees celcius
    int weathercode;        int 0-99 that has a corresponding text description, resolved in function WMOInterpreter(weathercode);
    char* description;      verbose weather description from OpenMeteo docs: https://open-meteo.com/en/docs#weather_variable_documentation
*/

WeatherReport Weather_GetReport(char* _CityName, double _Latitude, double _Longitude){
    char api_url[256]; /* "https://api.open-meteo.com/v1/forecast?latitude=%lf&longitude=%lf&current_weather=true&timezone=Europe/Stockholm"; */
    /* bool CallHTTP = false;*/ 
    cJSON* JsonRoot;
    
    /* determine json file name */
    size_t memoryNeeded = strlen(_CityName) + strlen("cachedreports/") + strlen(".json") + 1;
    char* JsonFileName = malloc(memoryNeeded); /* +4 for ".json" and +1 for null terminator */
    sprintf(JsonFileName, "cachedreports/%s.json", _CityName);
    
    sprintf(api_url, "https://api.open-meteo.com/v1/forecast?latitude=%.4lf&longitude=%.4lf&current_weather=true&timezone=Europe/Stockholm", _Latitude, _Longitude);
    
    printf("api_url: %s\n", api_url);
    
    /* placeholder new weatherreport */
    WeatherReport NewWeatherReport = {0};

    /* Check if weatherdata already has been downloaded in cache*/
    JsonRoot = Weather_CheckCache(JsonFileName);
    /* Otherwise contact open-meteo API */
    if(JsonRoot == NULL){
        Weather_MakeHTTPCall(api_url, &JsonRoot);

        /* store downloaded data in cache */
        int wjtfErrCode = Write_JSON_To_File(JsonFileName, JsonRoot);
        assert(wjtfErrCode == 0);
        /*
        printf("Calling weather service...\n");
        Http myHttp;
        Http_Response* Response = calloc(1, sizeof(Http_Response));

        int ErrCode = Http_Initialize(&myHttp);
        assert(ErrCode == 0);

        int perErrCode = Http_Perform(&myHttp, api_url, Response);
        if(perErrCode != 0){
            printf("Get_Weather_Report>> http-Perform, error-code: %i.\n", perErrCode);
            printf("Response->data content ( %i bytes): %s\n", Response->size, Response->data);
        }

        assert(perErrCode == 0);
        
        if (Response == NULL || Response->data == NULL || Response->size == 0) {
            fprintf(stderr, "Http_Perform returned success but response is empty.\n");
            Http_Dispose_Response(Response);
            
            free(NewWeatherReport);
            return NULL;
        }

        JsonRoot = cJSON_Parse(Response->data);
        int wjtfErrCode = Write_JSON_To_File(JsonFileName, JsonRoot);
        assert(wjtfErrCode == 0);
    
        Http_Dispose_Response(Response);
        Http_Dispose(&myHttp);
        free(Response);
        */
    }

    /* populate newWeatherReport */
    Weather_CreateReport(JsonRoot, &NewWeatherReport, _CityName, _Latitude, _Longitude);

    /* Cleanup*/
    cJSON_Delete(JsonRoot);
    free(JsonFileName);
    return NewWeatherReport;
}

void Weather_DestroyReport(WeatherReport* report) {
    if (!report) return;
    free(report->cityname);      /* if strdup'd */
    free(report->description);   /* if strdup'd */
    free(report);
}

void Weather_Print(WeatherReport* report)
{
    printf("\n\tCity:\t\t%s\n", report->cityname);
	printf("\tTemperature:\t%i °C\n", report->temperature);
	printf("\tWindspeed:\t%.2f m/s\n", report->windspeed);
	printf("\tWind direction:\t%s\n", report->windDirectionVerbose);
	printf("\tDescription:\t%s\n", report->description);
	printf("\tTime:\t\t%lld\n\n", report->timestamp);
}

cJSON* Weather_CheckCache(char* _FileName){
    
    if(DoesFileExist(_FileName)){
        /* printf("Cached report found.\n"); */
        struct stat fileInfo;
        if(stat(_FileName, &fileInfo) == -1){
            perror("error: stat if statement");
            return NULL;
        }
        else
        {
            /* printf("FileName: %s\n", _FileName); */
            /* printf("Last modified: %s", ctime(&fileInfo.st_mtime)); */
            time_t timeNow = time(NULL);
            double timeDifference = difftime(timeNow, fileInfo.st_mtime);
            if( timeDifference > 900){
                printf("Weather report has expired (cached %.0lf seconds ago).\n", timeDifference);
                return NULL;
            }else
            {
                printf("Accessing cached weather report (cached %.0lf seconds ago).\n", timeDifference);
                
                return Read_JSON_From_File(_FileName);
            }
        }
    }

    return NULL;
}

int Weather_MakeHTTPCall(char* _Url, cJSON** _JsonRootPtr){
        Http myHttp;
        Http_Response* Response = calloc(1, sizeof(Http_Response));

        int ErrCode = Http_Initialize(&myHttp);
        assert(ErrCode == 0);

        int perErrCode = Http_Perform(&myHttp, _Url, Response);
        if(perErrCode != 0){
            printf("Get_Weather_Report>> http-Perform, error-code: %i.\n", perErrCode);
            printf("Response->data content ( %i bytes): %s\n", Response->size, Response->data);
            Http_Dispose_Response(Response);
            Http_Dispose(&myHttp);
            free(Response);
            return -1;
        }

        assert(perErrCode == 0);
        
        if (Response == NULL || Response->data == NULL || Response->size == 0) {
            fprintf(stderr, "Http_Perform returned success but response is empty.\n");
            Http_Dispose_Response(Response);
            Http_Dispose(&myHttp);
            free(Response);
            return -1;
        }

        *(_JsonRootPtr) = cJSON_Parse(Response->data);
        /* clean up moved here */
        Http_Dispose_Response(Response);
        Http_Dispose(&myHttp);
        free(Response);
        return 0;
}

int Weather_CreateReport(cJSON* _JsonRoot, WeatherReport* _NewWeatherReport, char* _CityName, double _Latitude, double _Longitude){
    if(_JsonRoot == NULL)
    {
        printf("ERROR PARSING JSON.\n");
        const char* error_ptr = cJSON_GetErrorPtr();
        if(error_ptr!= NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
            cJSON_Delete(_JsonRoot);
            return -1;
        }
    }

    /* printf("%s", cJSON_Print(_JsonRoot)); */

    cJSON* currentWeatherBlob = cJSON_GetObjectItemCaseSensitive(_JsonRoot, "current_weather"); 
    cJSON* jsonTime = cJSON_GetObjectItem(currentWeatherBlob, "time");
    cJSON* jsonWindSpeed = cJSON_GetObjectItem(currentWeatherBlob, "windspeed");
    cJSON* jsonTemperature = cJSON_GetObjectItem(currentWeatherBlob, "temperature");
    cJSON* jsonWeatherCode = cJSON_GetObjectItem(currentWeatherBlob, "weathercode");
    cJSON* jsonWindDirection = cJSON_GetObjectItem(currentWeatherBlob, "winddirection");

/* // additional data
    cJSON *interval = cJSON_GetObjectItem(CurrentWeatherBlob, "interval");
    cJSON *is_day = cJSON_GetObjectItem(CurrentWeatherBlob, "is_day");
*/
    /* printf("%s", cJSON_Print(jsonTime)); */

    char* timeStr = cJSON_Print(jsonTime);
    long long TimeLong = StringTimeToLongLong(timeStr);
    free(timeStr);
    /* long long TimeLong = StringTimeToLongLong(cJSON_Print(jsonTime)); */
    float windSpeed = jsonWindSpeed->valuedouble *1000/3600;
    int windDirection = jsonWindDirection->valueint;
    int temperature = jsonTemperature->valuedouble + 0.5;
    int weatherCode = jsonWeatherCode->valueint;
    char* weatherVerbose = WMOInterpreter(weatherCode);
    char* windDirectionVerbose = Weather_WindDirectionInterpreter(windDirection);

    _NewWeatherReport->cityname = _CityName;
    _NewWeatherReport->latitude = _Latitude;
    _NewWeatherReport->longitude = _Longitude;
    _NewWeatherReport->timestamp = TimeLong;
    _NewWeatherReport->windspeed = windSpeed;
    _NewWeatherReport->temperature = temperature;
    _NewWeatherReport->weathercode = weatherCode;
    _NewWeatherReport->description = weatherVerbose;
    _NewWeatherReport->windDirectionVerbose = windDirectionVerbose;

    return 0;
}

char* Weather_WindDirectionInterpreter(int _WindDirection){

    if(_WindDirection < 20 || _WindDirection >= 340){
        return "North";
    }
        if(_WindDirection >= 20 && _WindDirection < 70){
        return "North-East";
    }
        if(_WindDirection >= 70 && _WindDirection < 110){
        return "East";
    }
        if(_WindDirection >= 110 && _WindDirection < 160){
        return "South-East";
    }
        if(_WindDirection >= 160 && _WindDirection < 200){
        return "South";
    }
        if(_WindDirection >= 200 && _WindDirection < 250){
        return "South-West";
    }
        if(_WindDirection >= 250 && _WindDirection < 290){
        return "West";
    }
        if(_WindDirection >= 290 && _WindDirection < 340){
        return "North-West";
    }
    return "Unable to determine wind direction";
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