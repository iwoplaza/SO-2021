#include "utils.h"
#include <stdlib.h>
#include <fcntl.h>

int open_resource(const char* res_name, const char* path, int flags, int mode)
{
    int fd = open(path, flags, mode);

    if (fd == -1)
    {
        fprintf(stderr, "Couldn't open %s: %s\n", res_name, path);
        exit(1);
    }
    else
    {
        printf("Opened %s successfully.\n", res_name);
    }

    return fd;
}

FILE* open_resource_std(const char* res_name, const char* path, const char* mode)
{
    FILE* resource = fopen(path, mode);

    if (resource == NULL)
    {
        fprintf(stderr, "Couldn't open %s: %s\n", res_name, path);
        exit(1);
    }

    return resource;
}