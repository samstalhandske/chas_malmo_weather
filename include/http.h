#ifndef HTTP_H
#define HTTP_H

#include <curl/curl.h>

typedef struct Http Http;
struct Http
{
    CURL* curl;
};

typedef struct {
    char* data;
    size_t size;
} Http_Response;

int HttpInitialize(Http* h);
int Perform(Http* h, const char* data, Http_Response* response);
void Http_Dispose_Response(Http_Response* response);
void Dispose(Http* h);


#endif