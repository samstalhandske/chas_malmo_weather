#include <stdio.h>
#include <stdbool.h>
#include <data.h>
#include <http.h>

int citySelect()
{
    int choice;

    listCities();
    printf("Enter a number to select city; ");
    int scanfResult = scanf("%d", &choice);
	if(scanfResult <= 0){
		return -1;
	}

    return choice;
}

int main() {
	bool programShouldExit = false;
	Http h;
	int errorCode = HttpInitialize(&h);
	
	printf("Welcome to this weather app\n");
	buildDatabase();
	
	while (programShouldExit == false) {
		int choice = citySelect();
		if(choice < 1 || choice > 16)
		{
			printf("Invalid selection!\n");
			continue;
		}
		int perErrorCode = Perform(&h, fetchUrl(choice));

	}
	h.Dispose(&h);
	return 0;
}
 