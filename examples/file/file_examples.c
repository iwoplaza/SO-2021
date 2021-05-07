void read_file_std()
{
    FILE* file = fopen(/*filepath*/, "r");

    if (file == NULL)
    {
        printf("Failed to open file: '%s'\n", /*filepath*/);
        return 1;
    }

    // Do something with the file.

    fclose(file);
}

#define MIN_BUFFER 32

/**
 * Reads a single string from a file (up until reaching a ' ' or '\n' symbol).
 */
const char* read_string(FILE* file_handle, int* line_number)
{
    int buffer_size = MIN_BUFFER;
    char* buffer = malloc(buffer_size * sizeof(char));
    int ch;
    int index = 0;

    bool token_has_started = false;

    while ((ch = fgetc(file_handle)) != EOF)
    {
        if (ch == '\n' || ch == ' ') {
            if (line_number != NULL && ch == '\n')
                *line_number += 1;

            if (token_has_started)
            {
                // If the token was already being read, we have reached it's end
                break;
            }
            else
            {
                // If the whitespace before the token hasn't ended, we continue
                // to the next character.
                continue;
            }
        }

        // We reached a non-whitespace character.
        token_has_started = true;
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