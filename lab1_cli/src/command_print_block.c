#include "command_print_block.h"
#include "cli_context.h"
#include "command.h"
#include <stdio.h>

int do_print_block(void* context, char* error_out)
{
    CLIContext_t* ctx = (CLIContext_t*) context;

    // Fetching the block index
    int block_index;
    scanf("%d", &block_index);

    int amount_of_blocks = ctx->get_num_of_blocks(ctx->table);
    if (block_index < 0 || block_index >= amount_of_blocks)
    {
        sprintf(error_out, "Block with index '%d' does not exist", block_index);
        return CMD_INVALID_ARGS;
    }

    ctx->print_block(ctx->table, block_index);

    return CMD_SUCCESS;
}