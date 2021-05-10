#include "pgm.h"
#include <string.h>

static char error_msg[256];

int getline_skipcomment(char** buffer, int* buffersize, FILE* file)
{
    int bytes_read;
    
    do
    {
        bytes_read = getline(buffer, buffersize, file);
        if (bytes_read == -1)
        {
            return -1;
        }
    }
    while (bytes_read == 0 || buffer[0] == '#');

    return bytes_read;
}

PGMStatus_t pgm_parse(FILE* in, PGMImage_t* image_dest)
{
    int buffersize = 128;
    char* line = malloc(sizeof(char) * buffersize);

    /** HEADER **/

    fscanf(in, "%s", line);

    if (strcmp(line, "P2") != 0)
    {
        sprintf(error_msg, "The header doesn't start with 'P2'.\n");
        return PGM_INVALID_FORMAT;
    }

    // Dimensions
    int bytes_read = getline_skipcomment(&line, &buffersize, in);
    if (bytes_read == -1)
    {
        sprintf(error_msg, "Expected dimensions metadata after 'P2' header.\n");
        return PGM_INVALID_FORMAT;
    }

    int width, height;
    sscanf(line, "%d %d", &width, &height);

    // Max value
    bytes_read = getline_skipcomment(&line, &buffersize, in);
    if (bytes_read == -1)
    {
        sprintf(error_msg, "Expected max value after dimensions metadata.\n");
        return PGM_INVALID_FORMAT;
    }

    int max_value;
    sscanf(line, "%d", &max_value);

    // Parsing values
    image_dest->width = width;
    image_dest->height = height;
    image_dest->values = malloc(sizeof(unsigned char) * width * height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            fscanf(in, "%d", &image_dest->values[y * width + x]);
        }
    }
}

PGMStatus_t pgm_write(FILE* out, const PGMImage_t* image_src)
{

}

void pgm_free(PGMImage_t* image)
{
    free(image->values);
}