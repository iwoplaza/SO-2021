#include "helper.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

void run();
int helper_take_pizza();

Helper_t* helper = NULL;

void on_shutdown();

void handle_interrupt()
{
    exit(0);
}

int main(int argc, char** argv)
{
    mark_start_time();

    atexit(on_shutdown);

    signal(SIGINT, handle_interrupt);
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("==DELIVERY== Initializing... ");
    helper = helper_init(false);
    if (helper == NULL)
    {
        print_error();
        return -1;
    }
    print_ok();

    // Running.
    run();

    // Cleaning up.
    on_shutdown();

    return 0;
}

void on_shutdown()
{
    if (helper != NULL)
    {
        helper_free(helper);
    }

    printf(COL_YELLOW "Delivery has finished execution...\n" COL_RESET);
}

_Noreturn void run()
{
    while (true)
    {
        int pizza_type = helper_take_pizza(helper);

        sleep_between_seconds(4, 5);

        print_common_info();
        printf("Dostarczam pizze: %d\n", pizza_type);

        sleep_between_seconds(4, 5);
    }
}

int helper_take_pizza()
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
        idx_to_take = (idx_to_take + 1) % TABLE_CAPACITY;
    }

    int pizza_type = shared_data->table[idx_to_take];
    shared_data->table[idx_to_take] = -1;

    print_common_info();
    printf("Pobieram pizze: %d Liczba pizz na stole: %d\n", pizza_type, get_pizzas_on_table(helper));

    // RETURN
    helper_return_access(helper, SEM_IDX_TABLE_ACCESS);

    return pizza_type;
}