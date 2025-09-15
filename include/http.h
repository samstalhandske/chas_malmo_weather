#ifndef HTTP_H
#define HTTP_H

#include <curl/curl.h>

typedef struct Http Http;
typedef struct Http
{
    CURL* curl;

    int (*Perform)(Http* h, const char* data);
    void (*Dispose)(Http* h);
}Http;

int HttpInitialize(Http* h);
int Perform(Http* h, const char* data);
void Dispose(Http* h);


#endif