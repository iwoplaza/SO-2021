#ifndef LAB1_CLI_COMMAND_H
#define LAB1_CLI_COMMAND_H

#define CMD_SUCCESS 0
#define CMD_INVALID_ARGS 1

typedef int (*CommandHandler_t)(void* context, char* error_out);

typedef struct Command_t {
    char name[32];
    char description[256];
    CommandHandler_t handler;
} Command_t;

#endif //LAB1_CLI_COMMAND_H
