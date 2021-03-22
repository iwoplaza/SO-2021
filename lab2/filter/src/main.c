#include <stdio.h>
#include <string.h>
#include <storeutils.h>
#include <time.h>

int main(int argc, char** argv)
{
    clock_t start = clock();

    if (argc != 3)
    {
        fprintf(stderr, "Expected exactly 2 arguments. Got %d", argc - 1);
        return 1;
    }

    if (strlen(argv[1]) != 1)
    {
        fprintf(stderr, "Expected first argument to be a single character.");
        return 1;
    }

    char char_to_find = argv[1][0];

    SULineNode_t* root = su_read_file(argv[2]);
    SULineNode_t* curr = root;

    while (curr != NULL)
    {
        char *char_ptr = strchr(curr->content, char_to_find);

        if (char_ptr != NULL)
        {
            printf("%s\n", curr->content);
        }

        curr = curr->next;
    }

    // Measuring the time
    clock_t delta = clock() - start;
    printf("time: %f ms", (double) delta / CLOCKS_PER_SEC * 1000);

    su_free_file(root);

    return 0;
}
