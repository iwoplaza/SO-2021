#include "cli_context.h"

#define COMMANDS 2

char cli_error[256];
bool running = true;
Command_t commands[COMMANDS];

void init();
void create_command(int index, const char *name, const char *description, CommandHandler_t handler);
void parse_command(CLIContext_t *context);

/* Uncontextual commands */
int do_help(void *context, char *error_out);
int do_exit(void *context, char *error_out);


int main(int argc, char **argv)
{
    init();

    CLIContext_t context;

    context.table = context.init_merge_table();
    context.table_size = -1;
    context.report_file = NULL;

    printf("===========================\n");
    printf("=========== CLI ===========\n");
    printf("===========================\n");
    printf("Welcome! Type 'help' for a list of available commands.\n\n");

    while (running)
    {
        parse_command(&context);
    }

    context.free_table(context.table);

    // Closing the report file.
    if (context.report_file != NULL)
    {
        fclose(context.report_file);
    }

    return 0;
}

/**
 * Initializes all commands.
 */
void init()
{
    create_command(0, "help", "Lists all available commands", do_help);
    create_command(1, "exit", "Exits the program", do_exit);
}

void create_command(int index, const char *name, const char *description, CommandHandler_t handler)
{
    strcpy(commands[index].name, name);
    strcpy(commands[index].description, description);
    commands[index].handler = handler;
}

void parse_command(CLIContext_t *context)
{
    printf(" > ");

    // Getting the command to handle
    char command_name[32];
    scanf("%s", command_name);

    // Handling the command
    bool ran_command = false;
    int status = CMD_SUCCESS;
    for (int i = 0; i < COMMANDS; ++i)
    {
        if (strcmp(command_name, commands[i].name) == 0)
        {
            status = commands[i].handler((void *) context, cli_error);
            ran_command = true;
        }
    }

    if (!ran_command)
    {
        printf("Unknown command: %s. Run 'help' to get the list of commands.\n", command_name);
    }

    if (status != CMD_SUCCESS)
    {
        printf("CLI—ERROR-%d: %s\n", status, cli_error);
    }
}

int do_help(void *context, char *error_out)
{
    for (int i = 0; i < COMMANDS; ++i)
    {
        printf("%16s - %s\n", commands[i].name, commands[i].description);
    }

    return CMD_SUCCESS;
}

int do_exit(void *context, char *error_out)
{
    running = false;

    return CMD_SUCCESS;
}