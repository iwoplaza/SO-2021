#include <stdio.h>
#include <unistd.h>


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