#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void sighandler(int signo, siginfo_t* siginfo, void* context)
{
    int value = siginfo->si_value.sival_int;
    if (signo == SIGUSR1)
    {
        printf("Received signal: SIGUSR1. Value: %d\n", value);
    }
    else
    {
        printf("Received signal: SIGUSR2. Value: %d\n", value);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;
    action.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &action, NULL) == -1)
    {
        fprintf(stderr, "Failed to setup handler for SIGUSR1\n");
    }

    if (sigaction(SIGUSR2, &action, NULL) == -1)
    {
        fprintf(stderr, "Failed to setup handler for SIGUSR1\n");
    }

    //-- zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1 i SIGUSR2 --
    // Blocking each signal in the set (the one in our case)
    sigset_t unblocked_set;
    // Wszystkie sygnały są blokowane.
    sigfillset(&unblocked_set);
    // Usuwamy nasze sygnały których nie chcemy blokować.
    sigdelset(&unblocked_set, SIGUSR1);
    sigdelset(&unblocked_set, SIGUSR2);
    // Ustawiamy maskę procesu na nowo wygenerowaną maskę.
    sigprocmask(SIG_SETMASK, &unblocked_set, NULL);

    //-- zdefiniuj obsluge SIGUSR1 i SIGUSR2 w taki sposob zeby proces potomny wydrukowal --
    //na konsole przekazana przez rodzica wraz z sygnalami SIGUSR1 i SIGUSR2 wartosci

    int child = fork();
    if (child == 0)
    {
        sleep(1);
    }
    else
    {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]

        int signo = atoi(argv[2]);
        union sigval value;
        value.sival_int = atoi(argv[1]);

        if (sigqueue(child, signo, value) == -1)
        {
            fprintf(stderr, "Failed to send signal.");
        }

        // Czekamy na proces dziecka.
        int status = 0;
        wait(&status);
    }

    return 0;
}
