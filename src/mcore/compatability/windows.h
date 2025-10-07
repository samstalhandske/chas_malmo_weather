#ifndef MCORE_WINDOWS_H
#define MCORE_WINDOWS_H

#include <wchar.h>

#ifdef _WIN32
	#define printf(x, y) wprintf(x, y)
#endif

#endif