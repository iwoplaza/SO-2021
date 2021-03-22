#include "command_save_block.h"
#include "cli_context.h"
#include "command.h"
#include <stdio.h>

#ifdef MEASURE_TIME
#include "timekeeping.h"
#endif

int do_save_block(void *context, char *error_out)
{
    CLIContext_t *ctx = (CLIContext_t *) context;

    // Fetching the parameters
    int block_index;
    char filename[256];
    scanf("%d %s", &block_index, filename);

    int amount_of_blocks = ctx->get_num_of_blocks(ctx->table);
    if (block_index < 0 || block_index >= amount_of_blocks)
    {
        sprintf(error_out, "Block with index '%d' does not exist", block_index);
        return CMD_INVALID_ARGS;
    }

#ifdef MEASURE_TIME
    clock_t start = clock();
#endif

    int status = ctx->write_block_to_file(ctx->table, block_index, filename);

#ifdef MEASURE_TIME
    clock_t delta = clock() - start;
    print_time(ctx, "save_block", ctx->get_num_of_rows(ctx->table, block_index), delta);
#endif

    if (status != 0)
    {
        sprintf(error_out, "save_block failed with error code: %d", status);
        return CMD_INVALID_ARGS;
    }

    return CMD_SUCCESS;
}