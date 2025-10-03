#include "console.h"
#include <stdio.h>

void Console_Flush()
{
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}