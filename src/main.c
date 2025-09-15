#include <stdio.h>
#include <stdbool.h>
#include <data.h>
#include <http.h>

int main() {
	bool programShouldExit = false;
	Http h;
	int errorCode = HttpInitialize(&h);
	
	printf("Welcome to this weather app\n");
	buildDatabase();
	
	while (programShouldExit == false) {
		int perErrorCode = Perform(&h, fetchUrl());
		
		

	}
	h.Dispose(&h);
	return 0;
}
 