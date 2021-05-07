#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


/*
 * Funkcja 'get_child_code' powinna poczekać na zakończenie dowolnego
 * procesu potomnego. Jeśli proces zakończył się poprawnie, powinna
 * zwrócić jego kod wyjścia, w przeciwnym wypadku -1. 
 */
int get_child_code(void) {
    int status = 0;

    if (wait(&status) == -1)
    {
        return -1;
    }

    return WEXITSTATUS(status);
}

void spawn_fib(int n) {
    pid_t id = fork();
    if (id == 0) {
        char buf[20];
        snprintf(buf, sizeof(buf), "%d", n);
        execl("main", "main", buf, NULL);
    }
}

int fib(int n) {
    if (n == 0 || n == 1) {
        return 1;
    } else {
        spawn_fib(n - 1);
        spawn_fib(n - 2);
        return get_child_code() + get_child_code();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fputs("Usage: ./main N\n", stderr);
        exit(-1);
    }
    int n = atoi(argv[1]);
    if (n > 11 || n < 0) {
        fprintf(stderr, "Argument out of range: %d\n", n);
        exit(-1);
    } else {
        return fib(n);
    }
}
