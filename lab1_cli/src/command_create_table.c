#include <string.h>
#include <stdio.h>
#include "command_create_table.h"
#include "command.h"
#include "cli_context.h"

int do_create_table(void* context, char* error_out)
{
    CLIContext_t* ctx = (CLIContext_t*) context;

    if (ctx->table_size != -1)
    {
        strcpy(error_out, "The table has already been created.");
        return CMD_INVALID_ARGS;
    }

    // Fetching the table size
    scanf("%d", &ctx->table_size);

    if (ctx->table_size <= 0)
    {
        strcpy(error_out, "The table size has to be positive");
        return CMD_INVALID_ARGS;
    }

    printf("Table size = %d\n", ctx->table_size);

    return CMD_SUCCESS;
}