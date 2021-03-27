#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Expected exactly one argument: the amount of child processes to make.\n");
        return 1;
    }

    int amount_of_children = atoi(argv[1]);

    if (amount_of_children <= 0)
        return 0;

    pid_t pid;
    do
    {
        // Creating a new child process.
        pid = fork();
    }
    while(pid != 0 /* are we still in the parent process */ && --amount_of_children > 0);

    // We are now either in the child process, or in the parent process after creating all the children.
    if (pid == 0)
    {
        printf("Hello from child process %d!\n", getpid());
    }

    return 0;
}
