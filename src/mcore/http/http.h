#ifndef HTTP_H
#define HTTP_H

#include <curl/curl.h>

typedef struct {

} HTTP;

typedef enum {
    INIT_CURL_OK,
    INIT_CURL_Failed_To_Initialize,
} Init_CURL_Response;

Init_CURL_Response init_curl();

/* Initialize */
/* Dispose */
/* Perform */

#endif