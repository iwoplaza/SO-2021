#include <stdio.h>
#include <mergeutils.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

void print_time(FILE* report_file, const char* label, int lines, clock_t delta)
{
    double miliseconds = (double) delta / CLOCKS_PER_SEC * 1000;
    printf("TIME: %-20s %10ld ticks, %10f ms\n", label, delta, miliseconds);

    char buffer[256];
    sprintf(buffer, "%s (%d lines)", label, lines);

    fprintf(report_file, "%-32s %10ld ticks, %10f ms\n", buffer, delta, miliseconds);
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

bool merge_pair(MUMergeTable_t* table, const char* filename_a, const char* filename_b)
{
    int merge_status = mu_merge_files(table, filename_a, filename_b);
    if (merge_status != 0)
    {
        fprintf(stderr, "merge_files failed with error code: %d", merge_status);
        return false;
    }

    return true;
}

bool do_merge_files(FILE* report_file, MUMergeTable_t* table)
{
    // Reading the amount of pairs
    int amount_of_pairs;
    scanf("%d", &amount_of_pairs);

    // Starting to measure time
    clock_t start = clock();

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

        if (!merge_pair(table, filename_a, filename_b))
        {
            return false;
        }
    }

    clock_t delta = clock() - start;
    int num_of_rows = mu_get_num_of_rows(table, mu_get_num_of_blocks(table) - 1);
    print_time(report_file, "merge_files", num_of_rows, delta);

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

    MUMergeTable_t* table = mu_init_merge_table();

    // Performing the merge
    do_merge_files(report_file, table);

    // Freeing the memory.
    mu_free_table(table);
    fclose(report_file);

    return 0;
}
