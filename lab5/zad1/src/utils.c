#include <stdlib.h>
#include "utils.h"

#define MIN_BUFFER 32

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

Token_t* read_tokens(FILE* file_handle)
{
    Token_t* head = NULL;
    Token_t* tail = NULL;

    const char* value = NULL;
    int line_number = 1;
    int curr_line = 1;
    while ((value = read_string(file_handle, &line_number)) != NULL)
    {
        Token_t* token = malloc(sizeof(Token_t));
        token->value = value;
        token->next = NULL;
        token->line = curr_line;

        if (head == NULL)
        {
            head = token;
            tail = token;
        }
        else
        {
            tail->next = token;
            tail = token;
        }

        curr_line = line_number;
    }

    return head;
}

void free_tokens(const Token_t* token_head)
{
    if (token_head == NULL)
        return;

    free((void*) token_head->value);

    free_tokens(token_head->next);

    free((void*) token_head);
}