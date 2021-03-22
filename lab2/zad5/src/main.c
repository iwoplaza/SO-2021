#include <stdio.h>
#include <storeutils.h>
#include <string.h>
#include <time.h>

#define BREAKING_POINT 4

SULineNode_t* wrap_lines(SULineNode_t* src, SULineNode_t** tail)
{
    SULineNode_t* root = NULL;

    const char* content_ptr = src->content;
    int content_len = strlen(content_ptr);
    while (content_len > BREAKING_POINT)
    {
        SULineNode_t* curr = su_create_line_node("");

        if (root == NULL)
        {
            root = curr;
        }
        else
        {
            (*tail)->next = curr;
        }

        *tail = curr;

        su_append_buffer_to_node(curr, content_ptr, BREAKING_POINT);
        content_ptr += BREAKING_POINT;
        content_len -= BREAKING_POINT;
    }

    // Creating a new node from the rest of the content
    SULineNode_t* curr = su_create_line_node(content_ptr);
    curr->next = src->next;

    if (root == NULL)
    {
        root = curr;
    }
    else
    {
        (*tail)->next = curr;
    }

    *tail = curr;

    return root;
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Expected exactly 2 arguments. Got %d", argc - 1);
        return 1;
    }

    clock_t start = clock();

    const char* src_filename = argv[1];
    const char* out_filename = argv[2];

    SULineNode_t* src_file = su_read_file(src_filename);

    if (src_file == NULL)
    {
        fprintf(stderr, "The source file is empty.");
        return 1;
    }

    SULineNode_t* curr;
    SULineNode_t* root_dst = wrap_lines(src_file, &curr);

    while (curr->next != NULL)
    {
        SULineNode_t* wrap_tail;
        SULineNode_t* curr_wrapped = wrap_lines(curr->next, &wrap_tail);
        curr->next = curr_wrapped;
        curr = wrap_tail;
    }

    su_write_to_file(root_dst, out_filename);

    // Measuring the time
    clock_t delta = clock() - start;
    printf("time: %f ms", (double) delta / CLOCKS_PER_SEC * 1000);

    su_free_file(src_file);
    su_free_file(root_dst);

    return 0;
}
