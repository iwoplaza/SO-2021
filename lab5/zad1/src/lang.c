#include <stdlib.h>
#include <string.h>
#include "lang.h"
#include "utils.h"
#include <stdbool.h>

static const char* PREFIX = "składnik";
static const int PREFIX_LEN = strlen("składnik");

// Function declarations

int parse_component_key(const char* string)
{
    if (strncmp(string, PREFIX, PREFIX_LEN) != 0)
    {
        return -1;
    }

    // Subtracting one, so that the component keys start from 0
    return atoi(string + PREFIX_LEN) - 1;
}

int find_argument_amount(const Token_t* curr)
{
    int amount = 0;

    int proper_line = curr->line;

    // We continue as long as there is a token, it's not a pipe and it's on the same line as all the other ones.
    while (curr != NULL && strcmp(curr->value, "|") != 0 && curr->line == proper_line)
    {
        amount++;
        curr = curr->next;
    }

    return amount;
}

Command_t* parse_command(InstructionSet_t* set, const Token_t** curr)
{
    if ((*curr) == NULL)
    {
        return NULL;
    }

    Command_t* command = malloc(sizeof(Command_t));
    command->pipes_to = NULL;

    // Retrieving the command name.
    command->command_name = malloc(strlen((*curr)->value) + 1);
    strcpy(command->command_name, (*curr)->value);

    // Retrieving the amount of arguments we need.
    command->arguments_amount = find_argument_amount(*curr) + 1;

    // Making space for the command name at the front, and the NULL on the back.
    char** arguments = malloc((command->arguments_amount) * sizeof(char*));
    arguments[0] = command->command_name;

    // Skipping past the command name.
    *curr = (*curr)->next;

    for (int i = 1; i < command->arguments_amount - 1; ++i)
    {
        char* value = malloc(strlen((*curr)->value) + 1);
        strcpy(value, (*curr)->value);
        arguments[i] = value;

        *curr = (*curr)->next;
    }

    arguments[command->arguments_amount - 1] = NULL;
    command->arguments = arguments;

    return command;
}

void parse_definition(InstructionSet_t* set, const Token_t** curr)
{
    int component_key = parse_component_key((*curr)->value);

    if (component_key == -1)
    {
        set->parse_status = PARSE_INVALID_SYNTAX;
        set->error_loc = (*curr)->line;
        sprintf(set->error_message, "Invalid component key: %s", (*curr)->value);
        return;
    }

    // Zooming past the component key and '=' operator.
    *curr = (*curr)->next->next;

    Command_t* command_head = NULL;
    Command_t* command_tail = NULL;

    Command_t* command_curr = NULL;

    int definition_line = (*curr)->line;
    while ((*curr) != NULL && (*curr)->line == definition_line && (command_curr = parse_command(set, curr)) != NULL)
    {
        if (command_head == NULL)
        {
            command_head = command_curr;
            command_tail = command_curr;
        }
        else
        {
            command_tail->pipes_to = command_curr;
            command_tail = command_curr;
        }

        // Skipping a pipe.
        if ((*curr) != NULL && strcmp((*curr)->value, "|") == 0)
        {
            *curr = (*curr)->next;
        }
    }

    set->components[set->components_amount] = command_head;
    set->components_amount++;

    // Reserving more space for future components.
    if (set->components_amount >= set->components_capacity)
    {
        set->components_capacity += 16;
        set->components = realloc(set->components, set->components_capacity * sizeof(Command_t*));
    }
}

void parse_instruction(InstructionSet_t* set, const Token_t** curr)
{
    PipeJobEntry_t* job_head = NULL;
    PipeJobEntry_t* job_tail = NULL;

    while (true)
    {
        int component_key = parse_component_key((*curr)->value);

        if (component_key == -1)
        {
            set->parse_status = PARSE_INVALID_SYNTAX;
            set->error_loc = (*curr)->line;
            sprintf(set->error_message, "Invalid component key: %s", (*curr)->value);
            return;
        }

        if (component_key >= set->components_amount)
        {
            set->parse_status = PARSE_ILLEGAL_ACCESS;
            set->error_loc = (*curr)->line;
            sprintf(set->error_message, "Tried to access missing component with key '%d'", component_key + 1);
            return;
        }

        PipeJobEntry_t* job = malloc(sizeof(PipeJobEntry_t));
        job->next = NULL;
        job->command = set->components[component_key];

        if (job_head == NULL)
        {
            job_head = job;
            job_tail = job;
        }
        else
        {
            job_tail->next = job;
            job_tail = job;
        }

        // If the next token is NULL or isn't a pipe, we don't have any more jobs to string together.
        if ((*curr)->next == NULL || strcmp((*curr)->next->value, "|") != 0)
        {
            *curr = (*curr)->next;
            break;
        }

        // Reaching past the '|' symbol.
        *curr = (*curr)->next->next;
    }

    set->jobs[set->jobs_amount] = job_head;
    set->jobs_amount++;

    // Reserving more space for future jobs.
    if (set->jobs_amount >= set->jobs_capacity)
    {
        set->jobs_capacity += 16;
        set->jobs = realloc(set->jobs, set->jobs_capacity * sizeof(PipeJobEntry_t*));
    }
}

const Token_t* parse_line(InstructionSet_t* set, const Token_t** curr)
{
    // We expect the passed in token to be the first one in the line.
    // If this line is a definition, the second character should be '=',
    // if not, we have an instruction on our hands.
    const Token_t* next_token = (*curr)->next;

    if (next_token == NULL || strcmp(next_token->value, "=") != 0)
    {
        // We have an instruction
        parse_instruction(set, curr);
    }
    else
    {
        // We have a definition
        parse_definition(set, curr);
    }

    return (*curr);
}

InstructionSet_t* init_instruction_set()
{
    InstructionSet_t* set = malloc(sizeof(InstructionSet_t));

    set->components_capacity = 32;
    set->components_amount = 0;
    set->components = malloc(set->components_capacity * sizeof(Command_t*));

    set->jobs_capacity = 32;
    set->jobs_amount = 0;
    set->jobs = malloc(set->jobs_capacity * sizeof(PipeJobEntry_t*));

    // Assuming the parsing is going to go successfully.
    set->parse_status = PARSE_SUCCESS;
    set->error_loc = -1;
    set->error_message = malloc(sizeof(char) * 256);

    return set;
}

void parse_file(InstructionSet_t* set, FILE* file_handle)
{
    const Token_t* token_head = read_tokens(file_handle);
    const Token_t* token_curr = token_head;

    // Evaluating the tokens
    while ((parse_line(set, &token_curr)) != NULL && set->parse_status == PARSE_SUCCESS);

    free_tokens(token_head);
}

void free_command(const Command_t* command)
{
    free((void*) command->command_name);

    for (int i = 1; i < command->arguments_amount - 1; ++i)
    {
        free((void*) command->arguments[i]);
    }
    free(command->arguments);

    // Freeing the command this command pipes to.
    if (command->pipes_to != NULL)
    {
        free_command((void*) command->pipes_to);
    }

    // Freeing the command itself
    free((void*) command);
}

void free_job(const PipeJobEntry_t* job)
{
    if (job == NULL)
        return;

    free_job(job->next);

    // Freeing the job itself
    free((void*) job);
}

void free_instruction_set(InstructionSet_t* set)
{
    free(set->error_message);

    for (int i = 0; i < set->components_amount; ++i)
    {
        free_command(set->components[i]);
    }
    free(set->components);

    for (int i = 0; i < set->jobs_amount; ++i)
    {
        free_job(set->jobs[i]);
    }
    free(set->jobs);
}

/*
 * PRINTING
 */

void print_component(Command_t* command)
{
    printf("\t(%#08x)", (unsigned int) command);

    while (command != NULL)
    {
        for (int i = 0; i < command->arguments_amount - 1; ++i)
        {
            printf(" %s", command->arguments[i]);
        }

        command = command->pipes_to;

        if (command != NULL)
        {
            printf(" |");
        }
    }

    printf("\n");
}

void print_job(PipeJobEntry_t* job)
{
    printf("\t");

    while (job != NULL)
    {
        printf(" %#08x", (unsigned int) job->command);

        job = job->next;

        if (job != NULL)
        {
            printf(" |");
        }
    }

    printf("\n");
}

void print_instruction_set(InstructionSet_t* set)
{
    printf("COMPONENTS:\n");
    for (int i = 0; i < set->components_amount; ++i)
    {
        print_component(set->components[i]);
    }

    printf("\n");
    printf("JOBS:\n");
    for (int i = 0; i < set->jobs_amount; ++i)
    {
        print_job(set->jobs[i]);
    }
}