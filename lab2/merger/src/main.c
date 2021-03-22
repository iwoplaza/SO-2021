#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mergeutils.h"

#define MAX_FILENAME_LEN 128

void prompt_filename(const char* label, char* dest);

int main(int argc, char** argv)
{
    clock_t start = clock();

    char filename_a[MAX_FILENAME_LEN];
    char filename_b[MAX_FILENAME_LEN];

    if (argc > 1)
        strcpy(filename_a, argv[1]);
    else
        prompt_filename("first", filename_a);

    if (argc > 2)
        strcpy(filename_b, argv[2]);
    else
        prompt_filename("second", filename_b);

    printf("File 1: %s\n", filename_a);
    printf("File 2: %s\n", filename_b);

    print_merged_files(filename_a, filename_b);

    // Measuring the time
    clock_t delta = clock() - start;
    printf("time: %f ms", (double) delta / CLOCKS_PER_SEC * 1000);

    return 0;
}

void prompt_filename(const char* label, char* dest)
{
    printf("Provide the path to the %s file: ", label);
    scanf("%127s", dest);
}