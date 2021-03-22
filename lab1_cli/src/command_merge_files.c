#include <string.h>
#include <stdio.h>
#include "command_merge_files.h"
#include "command.h"
#include "cli_context.h"

#ifdef MEASURE_TIME
#include "timekeeping.h"
#endif

int do_merge_files(void *context, char *error_out)
{
    CLIContext_t *ctx = (CLIContext_t *) context;

    if (ctx->table_size == -1)
    {
        strcpy(error_out, "create_table has to be called before calling anything else");
        return CMD_INVALID_ARGS;
    }

#ifdef MEASURE_TIME
    clock_t start = clock();
#endif

    char buffer[256];
    for (int i = 0; i < ctx->table_size; ++i)
    {
        scanf("%s", buffer);

        char *filename_a = buffer;
        char *colon = strchr(filename_a, ':');
        if (colon == NULL)
        {
            strcpy(error_out, "invalid pair format, should be file_a:file_b");
            return CMD_INVALID_ARGS;
        }

        // Replacing the colon with a null-terminator, so that filename_a can be
        // treated as it's own string.
        (*colon) = '\0';
        char *filename_b = colon + 1;
        printf("a: %s, b: %s\n", filename_a, filename_b);

        int merge_status = ctx->merge_files(ctx->table, filename_a, filename_b);
        if (merge_status != 0)
        {
            sprintf(error_out, "merge_files failed with error code: %d", merge_status);
            return CMD_INVALID_ARGS;
        }
    }

#ifdef MEASURE_TIME
    clock_t delta = clock() - start;
    int num_of_rows = ctx->get_num_of_rows(ctx->table, ctx->get_num_of_blocks(ctx->table) - 1);
    print_time(ctx, "merge_files", num_of_rows, delta);
#endif

    return CMD_SUCCESS;
}