#define LAB7_SYSTEMV
#include "helper.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

void run();
void place_oven(Helper_t* helper, int pizza_type, int* oven_idx);
void move_to_table(Helper_t* helper, int oven_idx);

bool running = true;

void handle_interrupt()
{
    running = false;
}

int main(int argc, char** argv)
{
    signal(SIGINT, handle_interrupt);
    srand(time(NULL));

    printf("==CHEF==\n\n");

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

    printf("Chef has finished execution...");

    return 0;
}

void run(Helper_t* helper)
{
    while (running)
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
    printf("Dodalem pizze: %d. Liczba pizz w piecu: %d\n", pizza_type, OVEN_CAPACITY - helper_get_sem(helper, SEM_IDX_OVEN_CAPACITY) + 1);

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

    int pizza_items = OVEN_CAPACITY - helper_get_sem(helper, SEM_IDX_OVEN_CAPACITY) + 1;

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
           helper_get_sem(helper, SEM_IDX_TABLE_ITEMS) - 1);

    // Giving back access to the table.
    helper_return_access(helper, SEM_IDX_TABLE_ACCESS);
}