#ifndef __STRDUP_H__
#define __STRDUP_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* strdupWSL(const char* str){
	char* copy = (char*)malloc(strlen(str) + 1);
	if(copy == NULL)
		return NULL;
	strcpy(copy, str);
	/*printf("[ALLOC] strdupWSL: %s -> %p\n", str, (void*)copy);*/
	return copy;
}
#endif