#include <stdio.h>

long long StringTimeToLongLong(char* _stringTime){
    long long year, month, day, hour, minute;
    sscanf(_stringTime, "%lld-%lld-%lldT%lld:%lld", &year, &month, &day, &hour, &minute);
    long long compact = year * 100000000 + month * 1000000 + day * 10000 + hour * 100 + minute;
    return compact;
}