#include <stdio.h>

int citySelect(){
    
    /* 	Asks the user to input a number.
    Will just return the number the user enters.
    citySelect(); */

    int city;

    printf("Enter a number to select city: ");
    scanf("%d", &city);

    return city;
}