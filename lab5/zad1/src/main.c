#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include "lang.h"

#define CLOSE_READ 1
#define CLOSE_WRITE 2

int get_amount_of_commands(PipeJobEntry_t* job_head)
{
    int amount_of_commands = 0;

    while (job_head != NULL)
    {
        Command_t* curr_command = job_head->command;
        while (curr_command != NULL)
        {
            amount_of_commands++;
            curr_command = curr_command->pipes_to;
        }
        job_head = job_head->next;
    }

    return amount_of_commands;
}

int** create_descriptor_table(int size)
{
    int** descriptors = malloc(sizeof(int*) * size);

    for (int i = 0; i < size - 1; ++i)
    {
        descriptors[i] = malloc(sizeof(int) * 2);
        if (pipe(descriptors[i]) == -1)
        {
            fprintf(stderr, "Couldn't create pipe descriptor.\n");
            exit(1);
        }
    }

    // The last entry in the array will describe both the input to the first program,
    // and the output of the last program in the chain.
    descriptors[size - 1] = malloc(sizeof(int) * 2);
    descriptors[size - 1][0] = STDIN_FILENO;
    descriptors[size - 1][1] = STDOUT_FILENO;

    return descriptors;
}

void close_pipe(int* pipe_desc, int mode)
{
    if ((mode & CLOSE_READ) && pipe_desc[0] != STDIN_FILENO)
        close(pipe_desc[0]);

    if ((mode & CLOSE_WRITE) && pipe_desc[1] != STDOUT_FILENO)
        close(pipe_desc[1]);
}

void perform_job(int job_index, PipeJobEntry_t* job)
{
    printf("\n== JOB %d:\n", job_index);

    int amount_of_commands = get_amount_of_commands(job);
    int** descriptors = create_descriptor_table(amount_of_commands);
    int index = 0;

    while (job != NULL)
    {
        Command_t *curr_command = job->command;

        while (curr_command != NULL)
        {
            // Wrapping around in the descriptor array.
            int prev_index = (index + amount_of_commands - 1) % amount_of_commands;
            int pid = fork();
            if (pid == 0)
            {
                // We are the child process.

                // Closing unused pipes.
                for (int i = 0; i < amount_of_commands; ++i)
                {
                    if (i != index && i != prev_index)
                        close_pipe(descriptors[i], CLOSE_READ | CLOSE_WRITE);
                }
                close_pipe(descriptors[index], CLOSE_READ);
                close_pipe(descriptors[prev_index], CLOSE_WRITE);

                dup2(descriptors[prev_index][0], STDIN_FILENO);
                dup2(descriptors[index][1], STDOUT_FILENO);

                execvp(curr_command->command_name, curr_command->arguments);
            }

            index++;
            curr_command = curr_command->pipes_to;
        }

        job = job->next;
    }

    // Closing unused pipes.
    for (int i = 0; i < amount_of_commands; ++i)
    {
        close_pipe(descriptors[i], CLOSE_READ | CLOSE_WRITE);
    }

    // We are the main process.
    // Waiting for child processes to end.
    int status = 0;
    while ((wait(&status)) > 0);

    // Freeing the descriptors.
    for (int i = 0; i < amount_of_commands; ++i)
    {
        free(descriptors[i]);
    }

    free(descriptors);
}

void perform_instructions(InstructionSet_t* set)
{
    for (int i = 0; i < set->jobs_amount; ++i)
    {
        perform_job(i, set->jobs[i]);
    }
}

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

    perform_instructions(set);

    fclose(file);
    free_instruction_set(set);

    return 0;
}