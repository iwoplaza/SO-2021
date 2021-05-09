#include <stdio.h>
#include <signal.h>

#ifndef LAB7_MAIN_NAME
    #define LAB7_MAIN_NAME "main_systemv"
    #define LAB7_CHEF_NAME "chef_systemv"
    #define LAB7_DELIVERY_NAME "delivery_systemv"
#endif

int main(int argc, char** argv)
{
    printf("==COORDINATOR==\n\n");

    if (argc != 3)
    {
        printf("USAGE: " LAB7_MAIN_NAME " amount_of_chefs amount_of_delivery");
    }

    int amount_of_chefs = atoi(argv[1]);
    int amount_of_delivery = atoi(argv[2]);

    if (amount_of_chefs < 0 || amount_of_delivery < 0)
    {
        fprintf(stderr, "The arguments have to be positive numbers.\n");
        return 1;
    }

    printf("Creating %d chefs and %d delivery workers.\n", amount_of_chefs, amount_of_delivery);

    int* chef_pids = malloc(sizeof(int) * amount_of_chefs);

    for (int i = 0; i < amount_of_chefs; ++i)
    {
        int pid = fork();
        if (pid == 0)
        {
            // We are the child.
            execl(LAB7_CHEF_NAME, LAB7_CHEF_NAME, NULL);
        }
        else
        {
            chef_pids[i] = pid;
        }
    }

    int* delivery_pids = malloc(sizeof(int) * amount_of_delivery);

    for (int i = 0; i < amount_of_delivery; ++i)
    {
        int pid = fork();
        if (pid == 0)
        {
            // We are the child.
            execl(LAB7_DELIVERY_NAME, LAB7_DELIVERY_NAME, NULL);
        }
        else
        {
            delivery_pids[i] = pid;
        }
    }

    pause();

    // Send an interrupt to every child process
    for (int i = 0; i < amount_of_chefs; ++i)
    {
        kill(chef_pids[i], SIGINT);
    }
    for (int i = 0; i < amount_of_delivery; ++i)
    {
        kill(delivery_pids[i], SIGINT);
    }

    // Waiting for child processes to end.
    int status = 0;
    while ((wait(&status)) > 0);

    return 0;
}