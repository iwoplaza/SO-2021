#define LAB7_SYSTEMV
#include "helper.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

void run();
int helper_take_pizza(Helper_t* helper);

bool running = true;

void handle_interrupt()
{
    running = false;
}

int main(int argc, char** argv)
{
    signal(SIGINT, handle_interrupt);
    srand(time(NULL));

    printf("==DELIVERY==\n\n");

    printf("Initializing... ");
    Helper_t* helper = helper_init(false);
    if (helper == NULL)
    {
        print_error();
        return -1;
    }
    print_ok();

    // Running.
    run();

    // Cleaning up.
    helper_free(helper);

    printf("Delivery has finished execution...");

    return 0;
}

void run(Helper_t* helper)
{
    while (running)
    {
        int pizza_type = helper_take_pizza(helper);

        sleep_between_seconds(4, 5);

        print_common_info();
        printf("Dostarczam pizze: %d\n", pizza_type);

        sleep_between_seconds(4, 5);
    }
}

int helper_take_pizza(Helper_t* helper)
{
    SharedMemoryData_t* shared_data = helper_get_shared_data(helper);

    //////////////////////////
    //     TAKING PIZZA     //
    //////////////////////////

    // We want to decrease the amount of items by taking a pizza.
    // This will block until items appear on the table.
    helper_sem_change(helper, SEM_IDX_TABLE_ITEMS, -1);

    // GET
    helper_request_access(helper, SEM_IDX_TABLE_ACCESS);

    // We do that after getting access to the table so that chefs don't put additional
    // items onto the table which could trigger other delivery men to act out of sync.
    helper_sem_change(helper, SEM_IDX_TABLE_CAPACITY, 1); // DOES NOT BLOCK

    // Looking for the pizza on the table.
    int idx_to_take = 0;
    while (shared_data->table[idx_to_take] == -1)
    {
        idx_to_take = idx_to_take % TABLE_CAPACITY;
    }

    int pizza_type = shared_data->table[idx_to_take];
    shared_data->table[idx_to_take] = -1;

    print_common_info();
    printf("Pobieram pizze: %d Liczba pizz na stole: %d\n", pizza_type, helper_get_sem(helper, SEM_IDX_TABLE_ITEMS) - 1);

    // RETURN
    helper_return_access(helper, SEM_IDX_TABLE_ACCESS);

    return pizza_type;
}