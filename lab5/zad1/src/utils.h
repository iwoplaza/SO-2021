#ifndef ZAD1_UTILS_H
#define ZAD1_UTILS_H

#include <stdio.h>

/**
 * Reads the file until it reaches either the end of the file, a newline ('\\n') or a space (' ') character.
 * @param file_handle The file to read from.
 * @returns A dynamically allocated string buffer. MANUAL CLEANUP NECESSARY.
 */
const char* read_string(FILE* file_handle);

#endif //ZAD1_UTILS_H
