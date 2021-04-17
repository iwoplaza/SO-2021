#include <stdio.h>
#include <unistd.h>

typedef struct Argument_t {
    const char* value;
    const struct Argument_t* next;
} Argument_t;

typedef struct Command_t {
    const char* command_name;
    const Argument_t* argument_head;
    const struct Command_t* pipes_to;
} Command_t;

typedef struct Component_t {
    const Command_t* command;
} Component_t;


Component_t* components;

void parse_component_line(const char* line)
{

}

void parse_file(FILE* file_handle)
{
    const char* line = read_string();
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

    execlp("ls --help", "ls");


    return 0;
}