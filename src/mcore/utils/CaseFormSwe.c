#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include "CaseFormSwe.h"

/*  
    Formats provided string to lower case, but formats first character to upper case
*/

void CaseFormatSwedish(char* input) {
    /*printf("Hello this is the start of the FormatUserInput function from stlc.h/stlc.c.\n");*/
    setlocale(LC_ALL, "");  /* Use system locale (must support UTF-8) */

    /* Convert char* to wchar_t* */
    size_t len = mbstowcs(NULL, input, 0);
    if (len == (size_t)-1) {
        fprintf(stderr, "Conversion to wide string failed.\n");
        return;
    }

    wchar_t *wide = malloc((len + 1) * sizeof(wchar_t));
    if (wide == NULL) return;

    mbstowcs(wide, input, len + 1);

    /* Lowercase each wide character */
    size_t i;
    for (i = 0; i < len; i++) {
        wide[i] = towlower(wide[i]);
    }
    wide[0] = towupper(wide[0]);

    /* Convert back to char* */
    size_t out_len = wcstombs(NULL, wide, 0);
    if (out_len == (size_t)-1) {
        fprintf(stderr, "Conversion back to multibyte failed.\n");
        free(wide);
        return;
    }

    wcstombs(input, wide, out_len + 1);  /* Overwrite original string */

    free(wide);
}


/* Newer version maybe better */
char* CaseFormatSwedishCopy(const char* input) {
    setlocale(LC_ALL, "");

    size_t len = mbstowcs(NULL, input, 0);
    if (len == (size_t)-1) return NULL;

    wchar_t* wide = malloc((len + 1) * sizeof(wchar_t));
    if (!wide) return NULL;

    mbstowcs(wide, input, len + 1);
    size_t i;
    for (i = 0; i < len; i++) wide[i] = towlower(wide[i]);
    wide[0] = towupper(wide[0]);

    size_t out_len = wcstombs(NULL, wide, 0);
    if (out_len == (size_t)-1) {
        free(wide);
        return NULL;
    }

    char* result = malloc(out_len + 1);
    if (!result) {
        free(wide);
        return NULL;
    }

    wcstombs(result, wide, out_len + 1);
    free(wide);
    return result;
}
