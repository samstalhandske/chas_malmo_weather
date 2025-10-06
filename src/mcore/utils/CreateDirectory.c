#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "strdup.h"


#ifdef _WIN32
#include <direct.h>  /* For _mkdir on Windows */
#define MKDIR(path) _mkdir(path)
#else
#include <unistd.h>  /* For POSIX mkdir */
#define MKDIR(path) mkdir(path, 0755)
#endif


/* creates directory in working directory, 
    usage: 
        CreateDirectory(MyNewDirectory) :: to create a single directory
        CreateDirectory(Dir1/Dir2/Dir3) :: becomes a directory tree
    returns 0 as success, -1 as failure (or directory already exists)
*/
int DirectoryCreate(const char* _Path) {
    struct stat st = {0};
    char* pathCopy = strdup(_Path);

    if (pathCopy == NULL) {
        printf("Failed to allocate memory for path copy.\n");
        return -1;
    }

    char fullPath[260] = {0};  /* Accumulates the full path */
    char* token = strtok(pathCopy, "/\\");

    while (token != NULL) {
        strcat(fullPath, token);
        strcat(fullPath, "/");  /*Append slash for next level*/

        if (stat(fullPath, &st) == -1) {
            if (MKDIR(fullPath) == 0) {
            } else {
                perror("mkdir failed");
                free(pathCopy);
                return -1;
            }
        }

        token = strtok(NULL, "/\\");
    }
    free(pathCopy);
    return 0;
}