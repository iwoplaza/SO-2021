#include <stdio.h>
#include "command_remove_block.h"
#include "command.h"
#include "cli_context.h"

#ifdef MEASURE_TIME
#include "timekeeping.h"
#endif

int do_remove_block(void* context, char* error_out)
{
    CLIContext_t* ctx = (CLIContext_t*) context;

    // Fetching the block index
    int block_index;
    scanf("%d", &block_index);

#ifdef MEASURE_TIME
    int num_of_rows = ctx->get_num_of_rows(ctx->table, block_index);
    clock_t start = clock();
#endif

    int status = ctx->remove_block(ctx->table, block_index);

#ifdef MEASURE_TIME
    clock_t delta = clock() - start;
    print_time(ctx, "remove_block", num_of_rows, delta);
#endif

    if (status != 0)
    {
        sprintf(error_out, "remove_block failed with error code: %d", status);
        return CMD_INVALID_ARGS;
    }

    return CMD_SUCCESS;
}