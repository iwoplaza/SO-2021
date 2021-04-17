#include <stdlib.h>
#include <string.h>
#include "lang.h"
#include "utils.h"
#include <stdbool.h>

typedef struct Token_t {
    const char* value;
    const struct Token_t* next;
    int line;
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

    int curr_line = 1;
    const char* value = NULL;
    bool end_of_line = false;
    while ((value = read_string(file_handle, &end_of_line)) != NULL)
    {
        Token_t* token = malloc(sizeof(Token_t));
        token->value = value;
        token->next = NULL;
        token->line = curr_line;

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

        if (end_of_line)
        {
            curr_line++;
        }
    }

    return head;
}

int parse_component_key(const char* string)
{
    static const char* PREFIX = "składnik";
    static const int PREFIX_LEN = strlen("składnik");

    if (strncmp(string, PREFIX, PREFIX_LEN) != 0)
    {
        return -1;
    }

    return atoi(string + PREFIX_LEN);
}

int find_argument_amount(const Token_t* curr)
{
    int amount = 0;

    while (curr != NULL && strcmp(curr->value, "|") != 0)
    {
        amount++;
        curr = curr->next;
    }

    return amount;
}

const Command_t* parse_command(InstructionSet_t* set, const Token_t** curr)
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
    *curr = (*curr)->next;

    // Retrieving the amount of arguments we need.
    command->arguments_amount = find_argument_amount(*curr) + 2;

    // Making space for the command name at the front, and the NULL on the back.
    char** arguments = malloc((command->arguments_amount) + sizeof(char*));
    arguments[0] = command->command_name;

    for (int i = 1; i < command->arguments_amount - 1; ++i)
    {
        char* value = malloc(strlen((*curr)->value) + 1);
        strcpy(value, (*curr)->value);
        arguments[i] = value;

        *curr = (*curr)->next;
    }

    arguments[command->arguments_amount - 1] = NULL;

    return command;
}

void parse_definition(InstructionSet_t* set, const Token_t** curr)
{
    int component_key = parse_component_key((*curr)->value);

    if (component_key == -1)
    {
        set->parse_status = PARSE_INVALID_SYNTAX;
        set->error_loc = (*curr)->line;
        set->error_message = "Invalid component key";
        return;
    }

    parse_command
}

void parse_instruction(InstructionSet_t* set, const Token_t** curr)
{

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
        parse_instruction(set, &curr);
    }
    else
    {
        // We have a definition
        parse_definition(set, &curr);
    }

    return curr;
}

InstructionSet_t* parse_file(FILE* file_handle)
{
    InstructionSet_t* iset = malloc(sizeof(InstructionSet_t));
    iset->components_capacity = 32;
    iset->components_amount = 0;
    iset->components = malloc(iset->components_capacity * sizeof(Command_t));

    // Assuming the parsing is going to go successfully.
    iset->parse_status = PARSE_SUCCESS;
    iset->error_loc = -1;
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
}

void free_instruction_set(InstructionSet_t* set)
{
    for (int i = 0; i < set->components_amount; ++i)
    {
        free_command(&set->components[i]);
    }
}