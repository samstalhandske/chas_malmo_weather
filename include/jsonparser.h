#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
  char time[32];
  int interval;
  float temperature;
  float windspeed;
  int winddirection;
  int is_day;
  int weathercode;
} CurrentWeather;

typedef struct {
  double latitude;
  double longitude;
  double generationtime_ms;
  int utc_offset_seconds;
  char timezone[16];
  char timezone_abbreviation[8];
  double elevation;
  CurrentWeather current_weather;
} Response;

int parse_response(const char *json, Response *resp);