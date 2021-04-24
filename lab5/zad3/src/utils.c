#include "utils.h"
#include <stdlib.h>

FILE* open_resource(const char* res_name, const char* path, const char* mode)
{
    FILE* resource = fopen(path, mode);

    if (resource == NULL)
    {
        fprintf(stderr, "Couldn't open %s: %s\n", res_name, path);
        exit(1);
    }
    else
    {
        printf("Opened %s successfully.\n", res_name);
    }

    return resource;
}