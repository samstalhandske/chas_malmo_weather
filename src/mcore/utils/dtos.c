#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* accepts double returns char* string. Caller must free this char* */

char* doubleToString(double value) {
    char temp[64];  /* Large enough for most double representations */
    sprintf(temp, "%.6f", value);  /* Format with 6 decimal places */

    /* Allocate memory for the result */
    char* str = malloc(strlen(temp) + 1);
    if (str != NULL) {
        strcpy(str, temp);
    }

    return str; 
}
