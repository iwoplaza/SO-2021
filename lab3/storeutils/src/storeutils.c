#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "storeutils.h"

#ifdef SU_SYS_SOLUTION
#include <fcntl.h>
#include <unistd.h>

typedef int FileHandle_t;
#else
typedef FILE* FileHandle_t;
#endif

#define SU_BUFFER_SIZE 256
static char BUFFER[SU_BUFFER_SIZE];

void su_resize_line(SULineNode_t* node, size_t content_size);
void su_append_to_node(SULineNode_t* node, const char* suffix);
SULineNode_t* su_append_line_after(SULineNode_t* node, const char* content);
SULineNode_t* su_create_line_node(const char* content);

/**
 * Returns the length of a line starting at the start of the BUFFER.
 * If the line doesn't end, the length returned will equal the buffer_size.
 * @param buffer
 * @param buffer_size
 * @return
 */
static int _get_line_length(char *buffer, int buffer_size)
{
    int length = 0;

    while (length < buffer_size && buffer[length] != '\n')
        length++;

    return length;
}

static SULineNode_t* _create_line_node()
{
    SULineNode_t* lineNode = malloc(sizeof(SULineNode_t));
    lineNode->capacity = 0;
    lineNode->content = NULL;
    lineNode->next = NULL;

    return lineNode;
}

static size_t _read_buffer(FileHandle_t file)
{
    #ifdef SU_SYS_SOLUTION
    return read(file, BUFFER, SU_BUFFER_SIZE);
    #else
    return fread(BUFFER, sizeof(char), SU_BUFFER_SIZE, file);
    #endif
}

static SULineNode_t* _read_file(FileHandle_t file)
{
    char* buffer_head = BUFFER;
    size_t bytes_left = _read_buffer(file);

    if (bytes_left == 0)
    {
        return NULL;
    }

    SULineNode_t* root = NULL;
    SULineNode_t* curr_node = NULL;

    while (bytes_left > 0)
    {
        int content_length = 0;

        if (curr_node == NULL)
        {
            curr_node = _create_line_node();
            root = curr_node;
        }
        else
        {
            curr_node->next = _create_line_node();
            curr_node = curr_node->next;
        }

        while (true)
        {
            int subline_length = _get_line_length(buffer_head, bytes_left);
            content_length += subline_length;

            su_resize_line(curr_node, content_length);
            memcpy(curr_node->content, buffer_head, subline_length);

            if (subline_length == bytes_left)
            {
                // We haven't reached the end of the line (might have reached the end of the file)
                buffer_head = BUFFER;
                bytes_left = _read_buffer(file);

                if (bytes_left == 0)
                {
                    break;
                }
            }
            else
            {
                buffer_head += subline_length + 1;
                bytes_left -= subline_length + 1;
                break;
            }
        }

        if (bytes_left == 0)
        {
            // Refilling the buffer
            buffer_head = BUFFER;
            bytes_left = _read_buffer(file);
        }
    }

    return root;
}

void su_resize_line(SULineNode_t* node, size_t content_size)
{
    if (node->content == NULL || content_size >= node->capacity)
    {
        node->capacity = content_size + 1;
        node->content = realloc(node->content, sizeof(char) * node->capacity);
        node->content[node->capacity - 1] = '\0';
    }
}

void su_append_buffer_to_node(SULineNode_t* node, const char* suffix, size_t size)
{
    size_t len = strlen(node->content);
    su_resize_line(node, len + size);
    memcpy(node->content + len, suffix, size);
}

void su_append_to_node(SULineNode_t* node, const char* suffix)
{
    su_append_buffer_to_node(node, suffix, strlen(suffix));
}

SULineNode_t* su_append_line_after(SULineNode_t* node, const char* content)
{
    SULineNode_t* curr = su_create_line_node(content);

    SULineNode_t* next = node->next;
    node->next = curr;
    curr->next = next;

    return curr;
}

SULineNode_t* su_create_line_node(const char* content)
{
    SULineNode_t* root = _create_line_node();
    size_t content_len = strlen(content);
    su_resize_line(root, content_len);
    memcpy(root->content, content, content_len);

    return root;
}

SULineNode_t* su_read_file(const char* filename)
{
    #ifdef SU_SYS_SOLUTION
    int handle = open(filename, O_RDONLY);

    if (handle == -1)
    {
        fprintf(stderr, "Couldn't open file '%s'", filename);
        exit(1);
    }
    #else
    FILE* handle = fopen(filename, "r");

    if (handle == NULL)
    {
        fprintf(stderr, "Couldn't open file '%s'", filename);
        exit(1);
    }
    #endif

    SULineNode_t* lineNode = _read_file(handle);

    #ifdef SU_SYS_SOLUTION
    close(handle);
    #else
    fclose(handle);
    #endif

    return lineNode;
}

void su_free_file(SULineNode_t* root)
{
    SULineNode_t* curr = root;

    while (curr != NULL)
    {
        SULineNode_t* next = curr->next;

        free(curr->content);

        curr = next;
    }
}

void su_write_to_file(SULineNode_t* root, const char* filename)
{
    #ifdef SU_SYS_SOLUTION
    int handle = creat(filename, 0);

    if (handle == -1)
    {
        fprintf(stderr, "Can't write to file '%s'", filename);
        exit(1);
    }

    SULineNode_t* curr = root;
    while (curr != NULL)
    {
        size_t line_len = strlen(curr->content);
        char* buffer = malloc(sizeof(char) * (line_len + 1));
        memcpy(buffer, curr->content, line_len);
        buffer[line_len] = '\n';

        write(handle, buffer, line_len + 1);

        free(buffer);

        curr = curr->next;
    }

    close(handle);
    #else
    FILE* handle = fopen(filename, "w");

    if (handle == NULL)
    {
        fprintf(stderr, "Can't write to file '%s'", filename);
        exit(1);
    }

    SULineNode_t* curr = root;
    while (curr != NULL)
    {
        fprintf(handle, "%s\n", curr->content);

        curr = curr->next;
    }

    fclose(handle);
    #endif
}