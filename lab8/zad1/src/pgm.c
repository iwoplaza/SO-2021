#include "pgm.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static char error_msg[256];

void skip_line(FILE* file)
{
    size_t buffer_size = 128;
    char* buffer = malloc(sizeof(char) * buffer_size);
    getline(&buffer, &buffer_size, file);

    free(buffer);
}

bool get_str(FILE* file, char* buffer)
{
    while (true)
    {
        int num_of_items = fscanf(file, "%s", buffer);

        if (num_of_items == 0)
        {
            return false;
        }

        if (buffer[0] != '#')
        {
            // We got an actual value.
            break;
        }

        // Skipping the whole rest of the line.
        skip_line(file);
    }

    return true;
}

PGMStatus_t pgm_parse(FILE* in, PGMImage_t* image_dest)
{
    size_t buffersize = 128;
    char* line = malloc(sizeof(char) * buffersize);

    /** HEADER **/
    if (!get_str(in, line) || strcmp(line, "P2") != 0)
    {
        sprintf(error_msg, "The header doesn't start with 'P2'.\n");
        return PGM_INVALID_FORMAT;
    }

    // Dimensions
    if (!get_str(in, line))
    {
        sprintf(error_msg, "Expected dimensions metadata after 'P2' header.\n");
        return PGM_INVALID_FORMAT;
    }
    int width = atoi(line);

    if (!get_str(in, line))
    {
        sprintf(error_msg, "Expected dimensions metadata after 'P2' header.\n");
        return PGM_INVALID_FORMAT;
    }
    int height = atoi(line);

    // Max value
    if (!get_str(in, line))
    {
        sprintf(error_msg, "Expected max value after dimensions metadata.\n");
        return PGM_INVALID_FORMAT;
    }
    int max_value = atoi(line);

    // Parsing values
    image_dest->width = width;
    image_dest->height = height;
    image_dest->max_value = max_value;
    image_dest->values = malloc(sizeof(unsigned char) * width * height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int value;
            fscanf(in, "%d", &value);

            image_dest->values[y * width + x] = (unsigned char) value;
        }
    }

    return PGM_SUCCESS;
}

PGMStatus_t pgm_write(FILE* out, const PGMImage_t* image_src)
{
    // Header
    fprintf(out, "P2\n");

    // Dimensions
    fprintf(out, "%d %d\n", image_src->width, image_src->height);

    // Max value
    fprintf(out, "%d\n", image_src->max_value);

    const int values_in_line = 17;
    int values_count = image_src->width * image_src->height;
    int line = 0;
    
    for (int i = 0; i < values_count; ++i)
    {
        fprintf(out, "%d ", image_src->values[i]);
        int new_line = i / values_in_line;

        if (new_line > line)
        {
            fprintf(out, "\n");
            line = new_line;
        }
    }

    return PGM_SUCCESS;
}

void pgm_free(PGMImage_t* image)
{
    free(image->values);
}

const char* pcm_get_error()
{
    return error_msg;
}