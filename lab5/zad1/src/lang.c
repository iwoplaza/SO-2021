#include <stdlib.h>
#include <string.h>
#include "lang.h"
#include "utils.h"

typedef struct Token_t {
    const char* value;
    const struct Token_t* next;
} Token_t;

// Function declarations

void free_tokens(const Token_t* token_head);

/**
 * Reads the file string-by-string and turns them into a linked list of tokens.
 * @param file_handle The file to read from
 * @return A linked list of tokens
 */
Token_t* read_tokens(FILE* file_handle)
{
    Token_t* head = NULL;
    Token_t* tail = NULL;

    const char* value = NULL;
    while ((value = read_string(file_handle)) != NULL)
    {
        Token_t* token = malloc(sizeof(Token_t));
        if (head == NULL)
        {
            head = token;
            tail = token;
        }
        else
        {
            tail->next = token;
            tail = token;
        }
    }

    return head;
}

const Token_t* parse_definition(InstructionSet_t* set, const Token_t* curr)
{
    // TODO Implement this
}

const Token_t* parse_instruction(InstructionSet_t* set, const Token_t* curr)
{
    // TODO Implement this
}

const Token_t* parse_line(InstructionSet_t* set, const Token_t* curr)
{
    // We expect the passed in token to be the first one in the line.
    // If this line is a definition, the second character should be '=',
    // if not, we have an instruction on our hands.
    const Token_t* next_token = curr->next;

    if (next_token == NULL || strcmp(next_token->value, "=") != 0)
    {
        // We have an instruction
        return parse_instruction(set, curr);
    }
    else
    {
        // We have a definition
        return parse_definition(set, curr);
    }
}

InstructionSet_t* parse_file(FILE* file_handle)
{
    InstructionSet_t* iset = malloc(sizeof(InstructionSet_t));
    iset->components_capacity = 32;
    iset->components_amount = 0;
    iset->components = malloc(iset->components_capacity * sizeof(Command_t));

    // Assuming the parsing is going to go successfully.
    iset->parse_status = PARSE_SUCCESS;
    iset->error_message = NULL;

    const Token_t* token_head = read_tokens(file_handle);
    const Token_t* token_curr = token_head;

    // Evaluating the tokens
    while ((token_curr = parse_line(iset, token_curr)) != NULL && iset->parse_status == PARSE_SUCCESS);

    free_tokens(token_head);

    return iset;
}

void free_tokens(const Token_t* token_head)
{
    if (token_head == NULL)
        return;

    free((void*) token_head->value);

    free_tokens(token_head->next);
}

void free_argument(const Argument_t* argument)
{
    if (argument == NULL)
        return;

    free((void*) argument->value);

    // Freeing the next argument in the chain.
    free_argument(argument->next);
}

void free_command(const Command_t* command)
{
    free((void*) command->command_name);

    free_argument(command->argument_head);

    // Freeing the command this command pipes to.
    free_command((void*) command->pipes_to);
}

void free_instruction_set(InstructionSet_t* set)
{
    for (int i = 0; i < set->components_amount; ++i)
    {
        free_command(&set->components[i]);
    }
}