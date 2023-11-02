#include <string.h> 

long long atoll(const char *nptr){
    return strtol (nptr, (char **) NULL, 10);
}
