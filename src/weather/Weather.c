#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>

#include "Weather.h"
#include "../mcore/utils/strdup.h"
#include "../mcore/json/cJSON/cJSON.h"
#include "../city/City.h"
#include "../mcore/utils/sttll.h"
#include "../mcore/json/fileHelper/fileHelper.h"
#include "../mcore/utils/CreateJsonCacheFilePath.h"
#include "../mcore/http/http.h"

/* ######################### Local functions ######################### */
cJSON* Weather_CheckCache(char* _FileName);
int Weather_MakeHTTPCall(char* _Url, cJSON** _JsonRootPtr);
int Weather_CreateReport(cJSON* _JsonRoot, WeatherReport* _NewWeatherReport, char* _CityName, double _Latitude, double _Longitude);
char* Weather_WindDirectionInterpreter(int _WindDirection);
char* WMOInterpreter(int _WMOCode);
/* ######################### Local functions ######################### */

/* Get weather report */
int Weather_GetReport(City* _City){
    char api_url[1024]; 

    cJSON* JsonRoot;
    
    /* determine json file name */
    char* JsonFileName = ConstructReportCacheFilePath(_City);

    /* construct api_url */
    sprintf(api_url, "https://api.open-meteo.com/v1/forecast?latitude=%.4lf&longitude=%.4lf&current=temperature_2m,relative_humidity_2m,"
        "apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,wind_speed_10m,wind_direction_10m,"
        "wind_gusts_10m,surface_pressure,pressure_msl&wind_speed_unit=ms", _City->latitude, _City->longitude);
    
    /* Check if weatherdata already has been downloaded in cache*/
    JsonRoot = Weather_CheckCache(JsonFileName);
    /* Otherwise contact open-meteo API */
    if(JsonRoot == NULL){
        printf("Retrieving new weather data...\n");
        Weather_MakeHTTPCall(api_url, &JsonRoot);
        
        /* store downloaded data in cache */
        int wjtfErrCode = Write_JSON_To_File(JsonFileName, JsonRoot);
        assert(wjtfErrCode == 0);
        
    }
    /* placeholder new weatherreport */
    WeatherReport* NewWeatherReport = malloc(sizeof(WeatherReport));
    if (NewWeatherReport == NULL) {
        perror("Failed to allocate memory: NewWeatherReport");
        return -1;
    }
    /* Parse Json data and populate WeatherReport */
    Weather_CreateReport(JsonRoot, NewWeatherReport, _City->displayName, _City->latitude, _City->longitude);

    /* if there already is a report, free that memory before attaching new report to LL Node */
    if(_City->WeatherData != NULL){
        Weather_DestroyReport(_City->WeatherData); 
    }
    /* attach WeatherReport to City Node*/
    _City->WeatherData = NewWeatherReport;

    /* Cleanup*/
    cJSON_Delete(JsonRoot);
    free(JsonFileName);
    return 0;
}

/* Prints LinkedList nodes */
void Weather_PrintReport(City* _City)
{
    printf("\n\tCity:\t\t\t%s (Lat: %.4f Lon: %.4f)\n", _City->WeatherData->cityname, _City->latitude, _City->longitude);
	printf("\tTemperature:\t\t%i %s\n", _City->WeatherData->temperature, _City->WeatherData->temperatueUnits);
	printf("\tWindspeed:\t\t%.2f %s\n", _City->WeatherData->windspeed, _City->WeatherData->windspeedUnits);
	printf("\tWind direction:\t\t%s\n", _City->WeatherData->windDirectionVerbose);
	printf("\tDescription:\t\t%s\n", _City->WeatherData->weathercodeVerbose);
    printf("\tPrecipitation:\t\t%.2f %s\n", _City->WeatherData->precipitation, _City->WeatherData->precipitationUnits);
	printf("\tTime stamp:\t\t%lld\n\n", _City->WeatherData->timeLongNumber + 200); /* +200 to turn GMT to SWE time */
}

/* Check if cached json data is less than 900 seconds old, returns NULL if cache is expired or does not exist */
cJSON* Weather_CheckCache(char* _FileName){
    
    if(DoesFileExist(_FileName)){
        struct stat fileInfo;
        if(stat(_FileName, &fileInfo) == -1){
            perror("error: stat if statement");
            return NULL;
        }

        time_t timeNow = time(NULL);
        double timeDifference = difftime(timeNow, fileInfo.st_mtime);
        if( timeDifference > 900){
            printf("Weather report has expired (cached %.0lf seconds ago).\n", timeDifference);
            return NULL;
        }
        printf("Accessing cached weather report (cached %.0lf seconds ago).\n", timeDifference);
        
        return Read_JSON_From_File(_FileName);
    }
    return NULL;
}

/* Calls API_URL and returns JSON object */
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

/* populates WeatherReport struct with data from json response or cached json file */
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

    cJSON* jsonElevation = cJSON_GetObjectItem(_JsonRoot, "elevation");
    cJSON* jsonCurrent = cJSON_GetObjectItemCaseSensitive(_JsonRoot, "current");
    cJSON* jsonCurrentUnits = cJSON_GetObjectItemCaseSensitive(_JsonRoot, "current_units");
    /* UNITS */
    cJSON* jsonTemperatureUnits = cJSON_GetObjectItem(jsonCurrentUnits, "temperature_2m");
    cJSON* jsonWindSpeedUnits = cJSON_GetObjectItem(jsonCurrentUnits, "wind_speed_10m");
    cJSON* jsonWindDirectionUnits = cJSON_GetObjectItem(jsonCurrentUnits, "wind_direction_10m");
    cJSON* jsonPrecipitationUnits = cJSON_GetObjectItem(jsonCurrentUnits, "precipitation");
    /* MEASUREMENTS */
    cJSON* jsonTime = cJSON_GetObjectItem(jsonCurrent, "time");
    cJSON* jsonWindSpeed = cJSON_GetObjectItem(jsonCurrent, "wind_speed_10m");
    cJSON* jsonTemperature = cJSON_GetObjectItem(jsonCurrent, "temperature_2m");
    cJSON* jsonWeatherCode = cJSON_GetObjectItem(jsonCurrent, "weather_code");
    cJSON* jsonWindDirection = cJSON_GetObjectItem(jsonCurrent, "wind_direction_10m");
    cJSON* jsonPrecipitation = cJSON_GetObjectItem(jsonCurrent, "precipitation");
    cJSON* jsonIsDay = cJSON_GetObjectItem(jsonCurrent, "is_day");

    float windSpeed = jsonWindSpeed->valuedouble;
    int windDirection = jsonWindDirection->valueint;
    int temperature = jsonTemperature->valuedouble + 0.5; /* round up to integer */
    int weatherCode = jsonWeatherCode->valueint;
    double precipitation = jsonPrecipitation->valuedouble;
    char* weatherVerbose = WMOInterpreter(weatherCode);
    char* windDirectionVerbose = Weather_WindDirectionInterpreter(windDirection);
    char* temperatureUnits = strdup(jsonTemperatureUnits->valuestring);
    char* windspeedUnits = strdup(jsonWindSpeedUnits->valuestring);
    char* windDirectionUnits = strdup(jsonWindDirectionUnits->valuestring);
    char* precipitationUnits = strdup(jsonPrecipitationUnits->valuestring);
    char* timeISO8601 = strdup(jsonTime->valuestring);
    long long TimeLong = StringTimeToLongLong(timeISO8601);

    bool isDay = false;
    if(jsonIsDay->valueint == 1) 
        isDay = true;

    _NewWeatherReport->cityname = _CityName;
    _NewWeatherReport->latitude = _Latitude;
    _NewWeatherReport->longitude = _Longitude;
    _NewWeatherReport->timeLongNumber = TimeLong;
    _NewWeatherReport->timeISO8601 = timeISO8601;
    _NewWeatherReport->windspeed = windSpeed;
    _NewWeatherReport->windspeedMS = windSpeed *1000/3600;
    _NewWeatherReport->temperature = temperature;
    _NewWeatherReport->weathercode = weatherCode;
    _NewWeatherReport->weathercodeVerbose = weatherVerbose;
    _NewWeatherReport->windDirection = windDirection;
    _NewWeatherReport->windDirectionVerbose = windDirectionVerbose;
    _NewWeatherReport->isDay = isDay;
    _NewWeatherReport->elevation = jsonElevation->valueint;
    _NewWeatherReport->temperatueUnits = temperatureUnits;
    _NewWeatherReport->windspeedKMPH = windSpeed;
    _NewWeatherReport->windspeedUnits = windspeedUnits;
    _NewWeatherReport->windDirectionUnits = windDirectionUnits;
    _NewWeatherReport->precipitation = precipitation;
    _NewWeatherReport->precipitationUnits = precipitationUnits;

    return 0;
}

/* Called upon during program shutdown */
void Weather_DestroyReport(WeatherReport* _Report) {
    if (_Report == NULL) return;
    free(_Report->timeISO8601);
    free(_Report->precipitationUnits);
    free(_Report->windDirectionUnits);
    free(_Report->temperatueUnits);
    free(_Report->windspeedUnits);
    free(_Report);
}

/* Interpret numerical wind direction to verbose description */
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

/* interpret numerical WMO weather code to verbose description */
char* WMOInterpreter(int _WMOCode){
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