#ifndef ZAD1_UTILS_H
#define ZAD1_UTILS_H

#include <stdio.h>
#include <stdbool.h>

/**
 * Reads the file until it reaches either the end of the file, a newline ('\\n') or a space (' ') character.
 * @param file_handle The file to read from.
 * @param new_line (optional). The value at the specified address will be true if the read caused us to reach the end of the line.
 * @returns A dynamically allocated string buffer. MANUAL CLEANUP NECESSARY.
 */
const char* read_string(FILE* file_handle, bool* end_of_line);

#endif //ZAD1_UTILS_H
