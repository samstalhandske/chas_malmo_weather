#include "http.h"

#include <curl/curl.h>

int HttpInitialize(Http* h)
{
    h->Perform = Perform;
    h->Dispose = Dispose;

    h->curl = curl_easy_init();

    if (h->curl == NULL)
    {
        printf("Error initializing curl\n");
        return -1;
    }

    return 0;
}

int Perform(Http* h, const char* data)
{
    curl_easy_setopt(h->curl, CURLOPT_URL, data);

    CURLcode code = curl_easy_perform(h->curl);

    if (code != CURLE_OK)
    {
        printf("Curl failed to perform\n");
        return -1;
    }

    return 0;
}

void Dispose(Http* h)
{
    curl_easy_cleanup(h->curl);
}