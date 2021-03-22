#include "mergeutils.h"
#include "storeutils.h"
#include <stdlib.h>
#include <stdio.h>

void print_merged_files(const char *filename_a, const char *filename_b)
{
    SULineNode_t* node_a = su_read_file(filename_a);
    SULineNode_t* node_b = su_read_file(filename_b);

    if (node_a == NULL || node_b == NULL)
    {
        fprintf(stderr, "Failed to read from one of the files.");
        su_free_file(node_a);
        su_free_file(node_b);
        exit(1);
    }

    SULineNode_t* curr_a = node_a;
    SULineNode_t* curr_b = node_b;
    while (curr_a != NULL || curr_b != NULL)
    {
        if (curr_a != NULL)
        {
            printf("%s\n", curr_a->content);
            curr_a = curr_a->next;
        }

        if (curr_b != NULL)
        {
            printf("%s\n", curr_b->content);
            curr_b = curr_b->next;
        }
    }

    su_free_file(node_a);
    su_free_file(node_b);
}