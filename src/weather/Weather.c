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
int Weather_MakeHTTPCall(char* _Url, cJSON* _JsonRoot);
int Weather_CreateReport(cJSON* _JsonRoot, WeatherReport* _NewWeatherReport, char* _CityName, double _Latitude, double _Longitude);
char* WMOInterpreter(int _WMOCode);
/* ######################### Local functions ######################### */

/* (PR) Saknar City object i main.c ? blir inget namn på staden just nu (City* _City)*/
/*
    char* cityname;         TODO (PR) Name of city, f.ex Malmö, Stockholm not yet implemented 
    double latitude;        the variable speaks for itself 
    double longitude;       the variable speaks for itself
    long long timestamp;    the time obtained from OpenMeteo API: YYYYMMDDHHMM
    double windspeed;       Windspeed is provided as m/s (API data is provided as km/h, APIData*1000/3600 resolves to m/s)
    int temperature;        rounded up or down from API Double, degrees celcius
    int weathercode;        int 0-99 that has a corresponding text description, resolved in function WMOInterpreter(weathercode);
    char* description;      verbose weather description from OpenMeteo docs: https://open-meteo.com/en/docs#weather_variable_documentation
*/

WeatherReport* Weather_GetReport(char* _CityName, double _Latitude, double _Longitude){
    char api_url[256]; /* "https://api.open-meteo.com/v1/forecast?latitude=%lf&longitude=%lf&current_weather=true"; */
    /* bool CallHTTP = false;*/ 
    cJSON* JsonRoot;
    
    /* determine json file name */
    size_t memoryNeeded = strlen(_CityName) + strlen("cachedreports/") + strlen(".json") + 1;
    char* JsonFileName = malloc(memoryNeeded); /* +4 for ".json" and +1 for null terminator */
    sprintf(JsonFileName, "cachedreports/%s.json", _CityName);
    
    sprintf(api_url, "https://api.open-meteo.com/v1/forecast?latitude=%.4lf&longitude=%.4lf&current_weather=true", _Latitude, _Longitude);
    
    /* printf("api_url: %s\n", api_url); */
    
    /* placeholder new weatherreport */
    WeatherReport* NewWeatherReport = malloc(sizeof(WeatherReport));
    if (NewWeatherReport == NULL) {
        fprintf(stderr, "Failed to allocate memory for Weather_Report.\n");
        return NULL;
    }

    /* Check if weatherdata already has been downloaded in cache*/
    JsonRoot = Weather_CheckCache(JsonFileName);
    /* Otherwise contact open-meteo API */
    if(JsonRoot == NULL){
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
    }

    /* populate newWeatherReport */
    Weather_CreateReport(JsonRoot, NewWeatherReport, _CityName, _Latitude, _Longitude);

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

cJSON* Weather_CheckCache(char* _FileName){
    
    if(DoesFileExist(_FileName)){
        printf("Cached report found.\n");
        struct stat fileInfo;
        if(stat(_FileName, &fileInfo) == -1){
            perror("error: stat if statement");
            return NULL;
        }
        else
        {
            printf("FileName: %s\n", _FileName);
            printf("Last modified: %s", ctime(&fileInfo.st_mtime));
            time_t timeNow = time(NULL);
            double timeDifference = difftime(timeNow, fileInfo.st_mtime);
            if( timeDifference > 900){
                printf("Cached weather Json has expired (%.0lf seconds past expiration).\n", timeDifference-900);
                return NULL;
            }else
            {
                printf("Accessing cached weather Json (cached %.0lf seconds ago).\n", timeDifference);
                
                return Read_JSON_From_File(_FileName);
            }
        }
    }

    return NULL;
}

int Weather_MakeHTTPCall(char* _Url, cJSON* _JsonRoot){
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

        _JsonRoot = cJSON_Parse(Response->data);
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
    cJSON* CurrentWeatherBlob = cJSON_GetObjectItemCaseSensitive(_JsonRoot, "current_weather"); 
    cJSON* jsonTime = cJSON_GetObjectItem(CurrentWeatherBlob, "time");
    cJSON* windspeed = cJSON_GetObjectItem(CurrentWeatherBlob, "windspeed");
    cJSON* jsonTemp = cJSON_GetObjectItem(CurrentWeatherBlob, "temperature");
    cJSON* weathercode = cJSON_GetObjectItem(CurrentWeatherBlob, "weathercode");

/* // additional data
    cJSON *interval = cJSON_GetObjectItem(CurrentWeatherBlob, "interval");
    cJSON *winddirection = cJSON_GetObjectItem(CurrentWeatherBlob, "winddirection");
    cJSON *is_day = cJSON_GetObjectItem(CurrentWeatherBlob, "is_day");
*/
    /* printf("%s", cJSON_Print(jsonTime)); */

    char* timeStr = cJSON_Print(jsonTime);
    long long TimeLong = StringTimeToLongLong(timeStr);
    free(timeStr);
    /* long long TimeLong = StringTimeToLongLong(cJSON_Print(jsonTime)); */
    float WindSpeed = windspeed->valuedouble *1000/3600;
    int Temperature = jsonTemp->valuedouble + 0.5;
    int WCode = weathercode->valueint;
    char* WeatherVerbose = WMOInterpreter(WCode);

    _NewWeatherReport->cityname = _CityName;
    _NewWeatherReport->latitude = _Latitude;
    _NewWeatherReport->longitude = _Longitude;
    _NewWeatherReport->timestamp = TimeLong;
    _NewWeatherReport->windspeed = WindSpeed;
    _NewWeatherReport->temperature = Temperature;
    _NewWeatherReport->weathercode = WCode;
    _NewWeatherReport->description = WeatherVerbose;

    return 0;
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