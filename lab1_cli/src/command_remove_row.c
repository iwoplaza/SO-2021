#include <stdio.h>
#include "command_remove_row.h"
#include "command.h"
#include "cli_context.h"

int do_remove_row(void* context, char* error_out)
{
    CLIContext_t* ctx = (CLIContext_t*) context;

    int block_index, row_index;
    scanf("%d %d", &block_index, &row_index);

    int status = ctx->remove_row(ctx->table, block_index, row_index);
    if (status != 0)
    {
        sprintf(error_out, "remove_row failed with error code: %d", status);
        return CMD_INVALID_ARGS;
    }

    return CMD_SUCCESS;
}