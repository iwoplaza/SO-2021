#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <stdlib.h>
#include "command.h"
#include "command_create_table.h"
#include "command_merge_files.h"
#include "command_remove_block.h"
#include "command_remove_row.h"
#include "command_print_block.h"
#include "command_save_block.h"
#include "command_begin_report.h"
#include "cli_context.h"

#ifndef DYN_LOAD
#include <mergeutils.h>
#endif

#define COMMANDS 9

char cli_error[256];
bool running = true;
Command_t commands[COMMANDS];

void init();

void *init_library(CLIContext_t *context);

void create_command(int index, const char *name, const char *description, CommandHandler_t handler);

void parse_command(CLIContext_t *context);

/* Uncontextual commands */

int do_help(void *context, char *error_out);

int do_exit(void *context, char *error_out);

int main(int argc, char **argv)
{
    init();

    CLIContext_t context;

    void* lib_handle = init_library(&context);
#ifdef DYN_LOAD
    if (lib_handle == NULL)
        return 1;
#endif

    context.table = context.init_merge_table();
    context.table_size = -1;
    context.report_file = NULL;

    printf("===========================\n");
    printf("======== MERGE CLI ========\n");
    printf("===========================\n");
    printf("Welcome! Type 'help' for a list of available commands.\n\n");

    while (running)
        parse_command(&context);

    context.free_table(context.table);

    // Closing the report file.
    if (context.report_file != NULL)
    {
        fclose(context.report_file);
    }

#ifdef DYN_LOAD
    dlclose(lib_handle);
#endif

    return 0;
}

/**
 * Initializes all commands.
 */
void init()
{
    create_command(0, "help", "Lists all available commands", do_help);
    create_command(1, "exit", "Exits the program", do_exit);
    create_command(2, "create_table", "Creates a table of size <table_size>", do_create_table);
    create_command(3, "merge_files",
                   "Merges <table_size> pairs of elements, each in following format: file_a:file_b",
                   do_merge_files);
    create_command(4, "remove_block",
                   "Removes block with the given <index>",
                   do_remove_block);
    create_command(5, "remove_row",
                   "From block of index <block_index> it removes row with the given <row_index>",
                   do_remove_row);
    create_command(6, "print_block",
                   "Prints a merge block with the given <block_index>",
                   do_print_block);
    create_command(7, "save_block",
                   "Writes block <block_index> to file with filename <filename>",
                   do_save_block);
    create_command(8, "begin_report",
                   "Begins measuring time of actions.",
                   do_begin_report);
}

#ifdef DYN_LOAD
void *load_library_method(void *lib_handle, const char *symbol_name)
{
    void *sym_handle = dlsym(lib_handle, symbol_name);

    if (sym_handle == NULL)
    {
        fprintf(stderr, "Failed to load a Merge-Utils library function: %s\n", symbol_name);
        fputs(dlerror(), stderr);
        exit(1);
    }

    return sym_handle;
}
#endif

void *init_library(CLIContext_t *context)
{
#ifdef DYN_LOAD
    void *lib_handle = dlopen("liblab1.so", RTLD_NOW);
    if (lib_handle == NULL)
    {
        printf("Failed to load the Merge-Utils library.\n");
        fputs(dlerror(), stderr);
        return NULL;
    }

    context->init_merge_table = load_library_method(lib_handle, "mu_init_merge_table");
    context->free_table = load_library_method(lib_handle, "mu_free_table");
    context->merge_files = load_library_method(lib_handle, "mu_merge_files");
    context->remove_block = load_library_method(lib_handle, "mu_remove_block");
    context->remove_row = load_library_method(lib_handle, "mu_remove_row");
    context->get_num_of_blocks = load_library_method(lib_handle, "mu_get_num_of_blocks");
    context->get_num_of_rows = load_library_method(lib_handle, "mu_get_num_of_rows");
    context->print_block = load_library_method(lib_handle, "mu_print_block");
    context->write_block_to_file = load_library_method(lib_handle, "mu_write_block_to_file");

    return lib_handle;
#else
    context->init_merge_table = (InitMergeTableFunction) mu_init_merge_table;
    context->free_table = (FreeTableFunction) mu_free_table;
    context->merge_files = (MergeFilesFunction) mu_merge_files;
    context->remove_block = (RemoveBlockFunction) mu_remove_block;
    context->remove_row = (RemoveRowFunction) mu_remove_row;
    context->get_num_of_blocks = (GetNumOfBlocksFunction) mu_get_num_of_blocks;
    context->get_num_of_rows = (GetNumOfRowsFunction) mu_get_num_of_rows;
    context->print_block = (PrintBlockFunction) mu_print_block;
    context->write_block_to_file = (WriteBlockToFileFunction) mu_write_block_to_file;

    return NULL;
#endif
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