#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void signal_handler()
{
    printf("HANDLER\n");
}

int main(int argc, char** argv)
{
    signal(SIGINT, signal_handler);
    printf("Hello World\n");

    sleep(5);
    printf("We out here.\n");

    return 0;
}