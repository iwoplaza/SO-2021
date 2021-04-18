#ifndef ZAD1_UTILS_H
#define ZAD1_UTILS_H

#include <stdio.h>
#include <stdbool.h>

typedef struct Token_t {
    const char* value;
    const struct Token_t* next;
    int line;
} Token_t;

/**
 * Reads the file until it reaches either the end of the file, a newline ('\\n') or a space (' ') character.
 * @param file_handle The file to read from.
 * @param line_number (optional). Gets incremented when the currently read token ends with a new-line.
 * @returns A dynamically allocated string buffer. MANUAL CLEANUP NECESSARY.
 */
const char* read_string(FILE* file_handle, int* line_number);

/**
 * Reads the file string-by-string and turns them into a linked list of tokens.
 * @param file_handle The file to read from
 * @return A linked list of tokens
 */
Token_t* read_tokens(FILE* file_handle);

void free_tokens(const Token_t* token_head);

#endif //ZAD1_UTILS_H
