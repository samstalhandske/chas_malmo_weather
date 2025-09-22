#include <stdio.h>
#include <stdbool.h>
#include <data.h>
#include <assert.h>

#include "mcore/http/http.h"

int main() {
	Init_CURL_Response response = init_curl();
	assert(response == INIT_CURL_OK);
	
	return 0;
}