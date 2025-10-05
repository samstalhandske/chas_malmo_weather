#ifndef HTTP_H
#define HTTP_H

#include <curl/curl.h>

typedef struct {
    CURL* curl;
} Http;

typedef enum {
    INIT_CURL_OK,
    INIT_CURL_Failed_To_Initialize,
} Init_CURL_Response;

Init_CURL_Response init_curl();

typedef struct 
{
    char* data;
    int size;
} Http_Response;

int Http_Initialize(Http* h);
int Http_Perform(Http* h, const char* data, Http_Response* response);
void Http_Dispose_Response(Http_Response* response);
void Http_Dispose(Http* h);


/* Initialize */
/* Dispose */
/* Perform */

#endif