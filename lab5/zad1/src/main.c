#include <stdio.h>
#include <unistd.h>
#include "lang.h"

int main(int argc, char **argv)
{
    printf("=================================\n");
    printf("======== CMD INTERPRETER ========\n");
    printf("=================================\n");

    if (argc != 2)
    {
        printf("To use the interpreter, please provide a path to a file with instructions.");
        return 0;
    }

    const char* filepath = argv[1];

    FILE* file = fopen(filepath, "r");

    if (file == NULL)
    {
        printf("Failed to open file: '%s'\n", filepath);
        return 1;
    }

    InstructionSet_t* set = init_instruction_set();

    parse_file(set, file);

    if (set->parse_status != PARSE_SUCCESS)
    {
        fprintf(stderr, "[Parsing error | Line %d]: %s\n", set->error_loc, set->error_message);
        fclose(file);
        free_instruction_set(set);
        return 1;
    }

    print_instruction_set(set);

    fclose(file);
    free_instruction_set(set);

    return 0;
}