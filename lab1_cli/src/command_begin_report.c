#include <string.h>
#include <stdio.h>
#include "command_begin_report.h"
#include "cli_context.h"
#include "command.h"

int do_begin_report(void *context, char *error_out)
{
#ifdef MEASURE_TIME
    CLIContext_t *ctx = (CLIContext_t *) context;

    char filename[128];
    scanf("%s", filename);
    ctx->report_file = fopen(filename, "w");

    if (ctx->report_file == NULL)
    {
        sprintf(error_out, "Couldn't create report file: %s", filename);
        return CMD_INVALID_ARGS;
    }

    return CMD_SUCCESS;
#else
    sprintf(error_out, "To do reports, make sure to enable the MEASURE_TIME definition flag.\n");
    return CMD_INVALID_ARGS;
#endif
}