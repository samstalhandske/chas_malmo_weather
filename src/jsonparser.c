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

static void skip_ws(const char **p) {
  while (isspace(**p)) (*p)++;
}

static int parse_string(const char **p, char *out, size_t maxlen) {
  skip_ws(p);
  if (**p != '"') return 0;
  (*p)++;
  size_t i = 0;
  while (**p && **p != '"' && i < maxlen-1) {
    out[i++] = *(*p)++;
  }
  out[i] = 0;
  if (**p == '"') (*p)++;
  return 1;
}

static int parse_number(const char **p, double *out) {
  skip_ws(p);
  char buf[64];
  size_t i = 0;
  while ((isdigit(**p) || **p == '.' || **p == '-' || **p == '+') && i < sizeof(buf)-1) {
    buf[i++] = *(*p)++;
  }
  buf[i] = 0;
  if (i == 0) return 0;
  *out = atof(buf);
  return 1;
}

static int parse_int(const char **p, int *out) {
  double d;
  if (!parse_number(p, &d)) return 0;
  *out = (int)d;
  return 1;
}

static int expect(const char **p, char c) {
  skip_ws(p);
  if (**p == c) {
    (*p)++;
    return 1;
  }
  return 0;
}

static int parse_current_weather(const char **p, CurrentWeather *cw) 
{
  if (!expect(p, '{')) return 0;
  
  while (1) 
  {
    char key[32];
    skip_ws(p);
    if (**p == '}') 
    { 
        (*p)++;
        break; 
    }
    if (!parse_string(p, key, sizeof(key))) return 0;
    skip_ws(p);
    
    if (!expect(p, ':')) return 0;
    
    if (strcmp(key, "time") == 0) 
    {
        if (!parse_string(p, cw->time, sizeof(cw->time))) return 0;
    } 
    else if (strcmp(key, "interval") == 0) 
    {
        if (!parse_int(p, &cw->interval)) return 0;
    } 
    else if (strcmp(key, "temperature") == 0) 
    {
      double temp;
      if (!parse_number(p, &temp)) return 0;
      cw->temperature = (float)temp;
    } 
    else if (strcmp(key, "windspeed") == 0) 
    {
      double ws;
      if (!parse_number(p, &ws)) return 0;
      cw->windspeed = (float)ws;
    } 
    else if (strcmp(key, "winddirection") == 0) 
    {
      if (!parse_int(p, &cw->winddirection)) return 0;
    } else if (strcmp(key, "is_day") == 0) 
    {
      if (!parse_int(p, &cw->is_day)) return 0;
    } 
    else if (strcmp(key, "weathercode") == 0) 
    {
      if (!parse_int(p, &cw->weathercode)) return 0;
    } 
    else 
    {
      // skip unknown value
      while (**p && **p != ',' && **p != '}') (*p)++;
    }
    skip_ws(p);
    if (**p == ',') (*p)++;
    else if (**p == '}') 
    { 
        (*p)++; 
        break; 
    }
  }
  return 1;
}

int parse_response(const char *json, Response *resp) {
  const char *p = json;
  if (!expect(&p, '{')) return 0;
  while (1) {
    char key[64];
    skip_ws(&p);
    if (*p == '}') { p++; break; }
    if (!parse_string(&p, key, sizeof(key))) return 0;
    skip_ws(&p);
    if (!expect(&p, ':')) return 0;
    if (strcmp(key, "latitude") == 0) {
      if (!parse_number(&p, &resp->latitude)) return 0;
    } else if (strcmp(key, "longitude") == 0) {
      if (!parse_number(&p, &resp->longitude)) return 0;
    } else if (strcmp(key, "generationtime_ms") == 0) {
      if (!parse_number(&p, &resp->generationtime_ms)) return 0;
    } else if (strcmp(key, "utc_offset_seconds") == 0) {
      if (!parse_int(&p, &resp->utc_offset_seconds)) return 0;
    } else if (strcmp(key, "timezone") == 0) {
      if (!parse_string(&p, resp->timezone, sizeof(resp->timezone))) return 0;
    } else if (strcmp(key, "timezone_abbreviation") == 0) {
      if (!parse_string(&p, resp->timezone_abbreviation, sizeof(resp->timezone_abbreviation))) return 0;
    } else if (strcmp(key, "elevation") == 0) {
      if (!parse_number(&p, &resp->elevation)) return 0;
    } else if (strcmp(key, "current_weather") == 0) {
      if (!parse_current_weather(&p, &resp->current_weather)) return 0;
    } else {
      // skip unknown object or value
      if (*p == '{') {
        int depth = 1;
        p++;
        while (*p && depth > 0) {
          if (*p == '{') depth++;
          else if (*p == '}') depth--;
          p++;
        }
      } else {
        while (*p && *p != ',' && *p != '}') p++;
      }
    }
    skip_ws(&p);
    if (*p == ',') p++;
    else if (*p == '}') { p++; break; }
  }
  return 1;
}

// Example usage
#ifdef TEST_JSONPARSER
int main() {
  const char *json = "{\"latitude\":55.606525,\"longitude\":13.002045,\"generationtime_ms\":0.04494190216064453,\"utc_offset_seconds\":0,\"timezone\":\"GMT\",\"timezone_abbreviation\":\"GMT\",\"elevation\":11.0,\"current_weather_units\":{\"time\":\"iso8601\",\"interval\":\"seconds\",\"temperature\":\"°C\",\"windspeed\":\"km/h\",\"winddirection\":\"°\",\"is_day\":\"\",\"weathercode\":\"wmo code\"},\"current_weather\":{\"time\":\"2025-09-17T08:45\",\"interval\":900,\"temperature\":16.2,\"windspeed\":29.9,\"winddirection\":264,\"is_day\":1,\"weathercode\":2}}";
  Response resp;
  if (parse_response(json, &resp)) {
    printf("Lat: %f, Lon: %f, Temp: %.1f\n", resp.latitude, resp.longitude, resp.current_weather.temperature);
  } else {
    printf("Parse error\n");
  }
  return 0;
}
#endif