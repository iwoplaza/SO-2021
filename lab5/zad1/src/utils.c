#include <stdlib.h>
#include "utils.h"

#define MIN_BUFFER 32

const char* read_string(FILE* file_handle, bool* end_of_line)
{
    int buffer_size = MIN_BUFFER;
    char* buffer = malloc(buffer_size * sizeof(char));
    int ch;
    int index = 0;

    while ((ch = fgetc(file_handle)) != EOF)
    {
        printf("%c", ch);

        if (ch == '\n' || ch == ' ') {
            if (end_of_line != NULL)
                *end_of_line = ch == '\n';

            break;
        }

        buffer[index] = (char) ch;
        index++;

        // Checking if we just reached past last char spot.
        // If we did, resize the buffer by one.
        // (We still need the space for the '\0' character).
        if (index > buffer_size - 1)
        {
            buffer_size += MIN_BUFFER;
            buffer = realloc(buffer, buffer_size);
        }
    }

    if (index == 0)
    {
        free(buffer);

        return NULL;
    }

    // Adding the 'end-of-string' symbol to the end of the buffer.
    buffer[index] = '\0';

    return buffer;
}
