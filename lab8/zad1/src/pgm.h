#ifndef PGM_H
#define PGM_H

#include <stdio.h>

typedef struct PGMImage_t {
    int width;
    int height;
    unsigned char max_value;
    unsigned char* values;
} PGMImage_t;

typedef enum PGMStatus_t {
    PGM_SUCCESS,
    PGM_INVALID_FORMAT,
} PGMStatus_t;

PGMStatus_t pgm_parse(FILE* in, PGMImage_t* image_dest);
PGMStatus_t pgm_write(FILE* out, const PGMImage_t* image_src);
void pgm_free(PGMImage_t* image);

#endif // PGM_H