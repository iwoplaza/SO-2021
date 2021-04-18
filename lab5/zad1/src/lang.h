#ifndef ZAD1_LANG_H
#define ZAD1_LANG_H

#include <stdio.h>

typedef enum ParseStatus_t {
    PARSE_SUCCESS = 0,
    PARSE_INVALID_SYNTAX,
    PARSE_ILLEGAL_ACCESS,
} ParseStatus_t;

typedef struct Command_t {
    char* command_name;
    char** arguments;
    int arguments_amount;
    struct Command_t* pipes_to;
} Command_t;

typedef struct PipeJobEntry_t {
    Command_t* command;
    struct PipeJobEntry_t* next;
} PipeJobEntry_t;

typedef struct InstructionSet_t {
    Command_t** components;
    int components_capacity;
    int components_amount;
    PipeJobEntry_t** jobs;
    int jobs_capacity;
    int jobs_amount;
    ParseStatus_t parse_status;
    int error_loc;
    char* error_message;
} InstructionSet_t;

InstructionSet_t* init_instruction_set();
void parse_file(InstructionSet_t* set, FILE* file_handle);
void free_instruction_set(InstructionSet_t* set);
void print_instruction_set(InstructionSet_t* set);

#endif //ZAD1_LANG_H
