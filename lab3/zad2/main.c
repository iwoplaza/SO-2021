#include <stdio.h>
#include <mergeutils.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/times.h>

//#define WRITE_TO_FILE
#define REMOVE_FROM_MEMORY

void print_time(FILE* report_file, const char* label, clock_t clock_delta, clock_t user_delta, clock_t system_delta)
{
    long clocks_per_sec = sysconf(_SC_CLK_TCK);
    double clock_ms = (double) clock_delta / clocks_per_sec * 1000;
    double user_ms = (double) user_delta / clocks_per_sec * 1000;
    double system_ms = (double) system_delta / clocks_per_sec * 1000;
    printf("TIME: %-20s (real) %10f ms, (user) %10f ms, (system) %10f ms\n", label, clock_ms, user_ms, system_ms);

    fprintf(report_file, "%-32s (real) %10f ms, (user) %10f ms, (system) %10f ms\n",
            label, clock_ms, user_ms, system_ms);
}

bool split_pair(char* buffer, char** filename_a, char** filename_b)
{
    *filename_a = buffer;
    char *colon = strchr(*filename_a, ':');
    if (colon == NULL)
    {
        fprintf(stderr, "invalid pair format, should be file_a:file_b");
        return false;
    }

    // Replacing the colon with a null-terminator, so that filename_a can be
    // treated as it's own string.
    (*colon) = '\0';
    *filename_b = colon + 1;

    return true;
}

bool merge_pair(const char* filename_a, const char* filename_b)
{
    // Creating a seperate merge table for each pair, so that we don't copy the heap each time we create a process.
    MUMergeTable_t* table = mu_init_merge_table();

    int merge_status = mu_merge_files(table, filename_a, filename_b);
    if (merge_status != 0)
    {
        fprintf(stderr, "merge_files failed with error code: %d", merge_status);
        return false;
    }

    #ifdef WRITE_TO_FILE
    char out_filename[256];
    sprintf(out_filename, "%d.txt", getpid());
    mu_write_block_to_file(table, mu_get_num_of_blocks(table) - 1, out_filename);
    #endif

    #ifdef REMOVE_FROM_MEMORY
    mu_remove_block(table, 0);
    #endif

    mu_free_table(table);
    return true;
}

bool do_merge_files(FILE* report_file)
{
    // Reading the amount of pairs
    int amount_of_pairs;
    scanf("%d", &amount_of_pairs);

    // Starting to measure time
    struct tms start;
    clock_t start_clock = times(&start);

    char buffer[256];
    for (int i = 0; i < amount_of_pairs; ++i)
    {
        scanf("%s", buffer);

        char* filename_a;
        char* filename_b;

        if (!split_pair(buffer, &filename_a, &filename_b))
        {
            return false;
        }

        printf("a: %s, b: %s\n", filename_a, filename_b);

        pid_t pid = fork();

        if (pid == 0)
        {
            if (!merge_pair(filename_a, filename_b))
            {
                exit(1);
            }
            // We don't want the child process to do any more work.
            exit(0);
        }
    }

    // Waiting for each child process to end.
    pid_t child_pid;
    int status = 0;
    while ((child_pid = wait(&status)) > 0)
    {
        if (status == 0)
        {
            printf("Child process %d ended with a SUCCESS.\n", child_pid);
        }
        else
        {
            fprintf(stderr, "Child process %d ended with a FAILURE.\n", child_pid);
        }
    }

    struct tms end;
    clock_t end_clock = times(&end);
    print_time(report_file, "merge_files",
               end_clock - start_clock,
               end.tms_cutime - start.tms_cutime,
               end.tms_cstime - start.tms_cstime);

    return true;
}

int main()
{
    FILE* report_file = fopen("report.txt", "w");
    if (report_file == NULL)
    {
        fprintf(stderr, "Couldn't open 'report.txt' file for writing.");
        return 1;
    }

    // Performing the merge
    do_merge_files(report_file);

    // Freeing the memory.
    fclose(report_file);

    return 0;
}
