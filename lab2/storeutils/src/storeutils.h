#ifndef STOREUTILS_STOREUTILS_H
#define STOREUTILS_STOREUTILS_H

#include <stddef.h>

typedef struct SULineNode_t
{
    char* content;
    int capacity;
    struct SULineNode_t* next;
} SULineNode_t;


SULineNode_t* su_read_file(const char* filename);
void su_append_buffer_to_node(SULineNode_t* node, const char* suffix, size_t size);
void su_append_to_node(SULineNode_t* node, const char* suffix);
SULineNode_t* su_append_line_after(SULineNode_t* node, const char* content);
SULineNode_t* su_create_line_node(const char* content);
void su_resize_line(SULineNode_t* node, size_t content_size);
void su_free_file(SULineNode_t* root);

void su_write_to_file(SULineNode_t* root, const char* filename);

#endif //STOREUTILS_STOREUTILS_H
