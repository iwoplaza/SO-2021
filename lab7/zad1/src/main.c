#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "helper.h"
#include "utils.h"

#ifndef LAB7_MAIN_NAME
    #define LAB7_MAIN_NAME "main_systemv"
    #define LAB7_CHEF_NAME "chef_systemv"
    #define LAB7_DELIVERY_NAME "delivery_systemv"
#endif

bool running = true;

void handle_interrupt()
{
    running = false;
}

int main(int argc, char** argv)
{
    signal(SIGINT, handle_interrupt);

    printf("==COORDINATOR==\n\n");

    if (argc != 3)
    {
        printf("USAGE: " LAB7_MAIN_NAME " amount_of_chefs amount_of_delivery");
        return 0;
    }

    int amount_of_chefs = atoi(argv[1]);
    int amount_of_delivery = atoi(argv[2]);

    if (amount_of_chefs < 0 || amount_of_delivery < 0)
    {
        fprintf(stderr, "The arguments have to be positive numbers.\n");
        return 1;
    }

    // Setting up IPC
    printf("Setting up IPC... ");
    Helper_t* helper = helper_init(true);
    if (helper == NULL)
    {
        print_error();
        return 1;
    }
    print_ok();

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

    helper_destroy(helper);
    helper_free(helper);

    return 0;
}