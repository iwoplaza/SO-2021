#ifndef ZAD1_LANG_H
#define ZAD1_LANG_H

#include <stdio.h>

typedef enum ParseStatus_t {
    PARSE_SUCCESS = 0,
    PARSE_INVALID_SYNTAX,
} ParseStatus_t;

typedef struct Command_t {
    char* command_name;
    char** arguments;
    int arguments_amount;
    struct Command_t* pipes_to;
} Command_t;

typedef struct InstructionSet_t {
    Command_t* components;
    int components_capacity;
    int components_amount;
    ParseStatus_t parse_status;
    int error_loc;
    char* error_message;
} InstructionSet_t;

InstructionSet_t* parse_file(FILE* file_handle);
void free_instruction_set(InstructionSet_t* set);

#endif //ZAD1_LANG_H
