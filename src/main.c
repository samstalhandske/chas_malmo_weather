#include <stdio.h>
#include <stdbool.h>
#include <data.h>
#include <http.h>
#include <assert.h>

#include "mcore/http/http.h"

int main() {
	bool programShouldExit = false;

	Http h;
	int errorCode = HttpInitialize(&h);
	assert(errorCode == 0);
	
	printf("Welcome to this weather app\n");
	buildDatabase();
	
	while (programShouldExit == false) {
		listCities();
		printf("Enter a number to select city; ");

		int choice;
		int scanfResult = scanf("%d", &choice);
		if(scanfResult <= 0){
			return -1;
		}
		
		if(choice < 1 || choice > 16) /* TODO: SS - Don't hardcode 16 here. */
		{
			printf("Invalid selection!\n");
			continue;
		}

		Http_Response response = {0};
		int perErrorCode = Perform(&h, fetchUrl(choice), &response);
		printf("Perform, error-code: %i.\n", perErrorCode);

		if (perErrorCode != 0) {
			continue;
		}
		
		printf("%s\n", response.data);
		Http_Dispose_Response(&response);
	}

	Dispose(&h);


	return 0;
}