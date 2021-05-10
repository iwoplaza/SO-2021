#include "helper.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

void run();
void place_oven(Helper_t* helper, int pizza_type, int* oven_idx);
void move_to_table(Helper_t* helper, int oven_idx);

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

    printf("==CHEF== Initializing... ");
    helper = helper_init(false);
    if (helper == NULL)
    {
        print_error();
        return -1;
    }
    print_ok();

    // Running.
    run(helper);

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

    printf(COL_YELLOW "Chef has finished execution...\n" COL_RESET);
}

_Noreturn void run()
{
    while (true)
    {
        int pizza_type = rand() % 10;

        print_common_info();
        printf("Przygotowuje pizze: %d\n", pizza_type);

        sleep_between_seconds(1, 2);

        int oven_idx;
        place_oven(helper, pizza_type, &oven_idx);

        sleep_between_seconds(4, 5);

        move_to_table(helper, oven_idx);
    }
}

void place_oven(Helper_t* helper, int pizza_type, int* oven_idx)
{

    // Reserving a space for the pizza. This is done before asking for access
    // because another chef could ask for access in order to remove a pizza,
    // while we'd be waiting for the capacity to change. We'd be locked.
    helper_sem_change(helper, SEM_IDX_OVEN_CAPACITY, -1);

    // Requesting access to the oven.
    helper_request_access(helper, SEM_IDX_OVEN_ACCESS);

    // Altering the shared memory.
    SharedMemoryData_t* shared_data = helper_get_shared_data(helper);

    // Traversing forward in search of a free spot.
    while (shared_data->oven[shared_data->oven_tail] != -1)
    {
        shared_data->oven_tail = (shared_data->oven_tail + 1) % OVEN_CAPACITY;
    }

    *oven_idx = shared_data->oven_tail;
    shared_data->oven[*oven_idx] = pizza_type;
    shared_data->oven_tail++;

    print_common_info();
    printf("Dodalem pizze: %d. Liczba pizz w piecu: %d\n", pizza_type, get_pizzas_in_oven(helper));

    // Giving back access to the oven.
    helper_return_access(helper, SEM_IDX_OVEN_ACCESS);
}

void move_to_table(Helper_t* helper, int oven_idx)
{
    SharedMemoryData_t* shared_data = helper_get_shared_data(helper);

    //////////////////////////
    //     TAKING PIZZA     //
    //////////////////////////

    // Requesting access to the oven.
    helper_request_access(helper, SEM_IDX_OVEN_ACCESS);

    // Increasing the capacity, because we're taking a pizza from the oven.
    helper_sem_change(helper, SEM_IDX_OVEN_CAPACITY, 1);

    int pizza_type = shared_data->oven[oven_idx];
    shared_data->oven[oven_idx] = -1;

    int pizza_items = get_pizzas_in_oven(helper);

    helper_return_access(helper, SEM_IDX_OVEN_ACCESS);

    //////////////////////////
    //   PLACING ON TABLE   //
    //////////////////////////

    // Reserving a space for the pizza. This is done before asking for access
    // because a delivery worker could ask for access in order to remove a pizza,
    // while we'd be waiting for the capacity to change. We'd be locked.
    helper_sem_change(helper, SEM_IDX_TABLE_CAPACITY, -1);

    // Requesting access to the table.
    helper_request_access(helper, SEM_IDX_TABLE_ACCESS);

    // Adding an item on the table.
    int pizzas_on_table = get_pizzas_on_table(helper);
    helper_sem_change(helper, SEM_IDX_TABLE_ITEMS, 1);

    // Traversing forward in search of a free spot.
    while (shared_data->table[shared_data->table_tail] != -1)
    {
        shared_data->table_tail = (shared_data->table_tail + 1) % TABLE_CAPACITY;
    }
    int table_idx = shared_data->table_tail;
    shared_data->table[table_idx] = pizza_type;
    shared_data->table_tail++;

    print_common_info();
    printf("Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n",
           pizza_type,
           pizza_items,
           pizzas_on_table + 1);

    // Giving back access to the table.
    helper_return_access(helper, SEM_IDX_TABLE_ACCESS);
}