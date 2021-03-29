#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <storeutils.h>
#include <dirent.h>
#include <unistd.h>
#include <wait.h>

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

bool does_file_meet_conditions(const char* filename, const char* pattern)
{
    if(strlen(filename) <= 4 || strcmp(filename + strlen(filename) - 4, ".txt") != 0)
    {
        // The filename does not end in .txt
        return false;
    }

    SULineNode_t* root = su_read_file(filename);

    if (root == NULL)
    {
        // The file is empty.
        return false;
    }

    SULineNode_t* curr = root;
    while (curr != NULL)
    {
        if (find_occurrence(curr->content, pattern) != NULL)
        {
            return true;
        }
        curr = curr->next;
    }

    // Freeing the memory.
    su_free_file(root);

    return false;
}

void traverse_directory(const char* starting_dir, const char* pattern, int max_search_depth, const char* curr_dir, int indent)
{
    // Computing the indent string
    if (indent > 127)
        indent = 127;

    char indent_str[128];
    for (int i = 0; i < indent; ++i)
    {
        indent_str[i] = '\t';
    }
    indent_str[indent] = '\0';

    // Fetching elements in the starting directory.
    DIR* d;
    struct dirent* dir;
    d = opendir(starting_dir);

    if (d != NULL)
    {
        char path_buffer[256];
        while ((dir = readdir(d)) != NULL)
        {
            // Skipping the current and parent directories.
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;

            // Creating a full path to the element.
            sprintf(path_buffer, "%s/%s", starting_dir, dir->d_name);

            if (dir->d_type == DT_DIR)
            {
                // We execute a child process for the sub directory.
                pid_t pid = fork();
                if (pid == 0)
                {
                    // Generating arguments for the child execution.
                    char next_dir[256];
                    sprintf(next_dir, "%s%s/", curr_dir, dir->d_name);

                    char msd_str[256];
                    sprintf(msd_str, "%d", max_search_depth - 1);

                    char next_indent_str[256];
                    sprintf(next_indent_str, "%d", indent + 1);

                    // Printing the appropriate search tree entry to the console.
                    printf("%s[%s] (%d)\n", indent_str, next_dir, getpid());

                    // Executing the program recursively.
                    execl("./zad3", "zad3", path_buffer, pattern, msd_str, next_dir, next_indent_str, NULL);
                }
                else
                {
                    // Waiting for child process to end.
                    int status = 0;
                    wait(&status);
                }
            }
            else
            {
                // We check if the file meets out conditions.
                if (does_file_meet_conditions(path_buffer, pattern))
                {
                    printf("%s%s (found)\n", indent_str, dir->d_name);
                }
                else
                {
                    printf("%s%s\n", indent_str, dir->d_name);
                }
            }
        }

        closedir(d);
    }
    else
    {
        fprintf(stderr, "Couldn't access directory: %s", starting_dir);
    }
}

int main(int argc, char** argv)
{
    if (argc <= 3)
    {
        printf("Usage: zad3 <starting_dir> <pattern> <max_search_depth> [curr_dir] [indent]\n");
        return 0;
    }

    ///////////////////////////////
    /// Fetching program parameters
    ///////////////////////////////

    const char* starting_dir = argv[1];
    const char* pattern = argv[2];
    int max_search_depth = atoi(argv[3]);

    const char* curr_dir = "";
    if (argc >= 5)
    {
        curr_dir = argv[4];
    }

    int indent = 0;
    if (argc >= 6)
    {
        indent = atoi(argv[5]);
    }

    // Exit early
    if (max_search_depth < 0)
        return 0;

    ///////////////////////////////
    /// Fetching program parameters
    ///////////////////////////////
    traverse_directory(starting_dir, pattern, max_search_depth, curr_dir, indent);
}
