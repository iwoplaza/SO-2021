#include <stdio.h>
#include <storeutils.h>
#include <string.h>
#include <time.h>

const char* find_occurrence(const char* str, const char* pattern)
{
    while (*str != '\0')
    {
        const char* str_ptr = str;
        const char* pattern_ptr = pattern;

        while (*str_ptr == *pattern_ptr)
        {
            if (*(pattern_ptr+1) == '\0')
                return str;

            str_ptr++;
            pattern_ptr++;
        }

        str++;
    }

    return NULL;
}

SULineNode_t* replace(SULineNode_t* node, SULineNode_t* prev, const char* pattern, const char* repl)
{
    SULineNode_t* new_node = su_create_line_node("");
    new_node->next = node->next;

    size_t pattern_size = strlen(pattern);
    size_t repl_size = strlen(repl);

    const char* content_ptr = node->content;
    const char* occurrence_ptr = find_occurrence(content_ptr, pattern);
    while (occurrence_ptr != NULL)
    {
        // Appending the content before.
        su_append_buffer_to_node(new_node, content_ptr, occurrence_ptr - content_ptr);

        // Appending the replacement.
        su_append_buffer_to_node(new_node, repl, repl_size);

        content_ptr = occurrence_ptr + pattern_size;
        occurrence_ptr = find_occurrence(content_ptr, pattern);
    }

    if (prev != NULL)
    {
        prev->next = new_node;
    }

    return new_node;
}

int main(int argc, char** argv)
{
    if (argc != 5)
    {
        fprintf(stderr, "Expected exactly 4 arguments. Got %d", argc - 1);
        return 1;
    }

    clock_t start = clock();

    const char* pattern = argv[3];
    const char* replacement = argv[4];

    SULineNode_t* root_src = su_read_file(argv[1]);

    if (root_src == NULL)
    {
        fprintf(stderr, "The source file is empty.");
        return 1;
    }

    SULineNode_t* root_dst = replace(root_src, NULL, pattern, replacement);

    SULineNode_t* curr = root_dst;
    while (curr->next != NULL)
    {
        curr = replace(curr->next, curr, pattern, replacement);
    }

    su_write_to_file(root_dst, argv[2]);

    // Measuring the time
    clock_t delta = clock() - start;
    printf("time: %f ms", (double) delta / CLOCKS_PER_SEC * 1000);

    su_free_file(root_dst);
    su_free_file(root_src);

    return 0;
}
