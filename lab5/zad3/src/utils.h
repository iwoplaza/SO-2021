#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

int open_resource(const char* res_name, const char* path, int flags, int mode);

FILE* open_resource_std(const char* res_name, const char* path, const char* mode);


#endif // UTILS_H